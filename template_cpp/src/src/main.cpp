#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <string>
#include <sstream>
#include <list>

#include "parser.hpp"
#include "hello.h"
#include <signal.h>

#include "receiver.hpp"
#include "sender.hpp"
#include "perfectlink.hpp"
#include "threadsafelist.hpp"
#include "beb.hpp"
//#include "rb.hpp"
#include "urb.hpp"
#include "fifo.hpp"
#include "lcb.hpp"


sockaddr_in createAddress(in_addr_t ip, unsigned short port);
void stopProcesses();
std::list<std::string> deconcatMessages(std::list<std::string> log);
void writeOutput();

std::string outputPath;

Receiver* this_process = nullptr;
std::vector<Sender*> senders;
std::vector<Perfectlink*> links;
BestEffortBroadcast* beb = nullptr;
UniformReliableBroadcast* urb = nullptr;
Broadcast* broadcast = nullptr;


sockaddr_in createAddress(in_addr_t ip, unsigned short port) {
    
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = ip;
  address.sin_port = port;
  memset(address.sin_zero, '\0', sizeof(address.sin_zero));

  return address;
}

void stopProcesses() {

  if (this_process != nullptr){
    this_process->setCanReceive(false);
  }

  for (auto& link: links) {

    if (link != nullptr) {
      link->setLinkInactive();
    }
  }

  if (beb != nullptr) {
    beb->setBroadcastInactive();
  }

  if (urb != nullptr) {
    urb->setBroadcastInactive();
  }

  if (broadcast != nullptr) {
    broadcast->setBroadcastInactive();
  }

}

std::list<std::string> deconcatMessages(std::list<std::string> log) {

  std::list<std::string> messages_deconcat;

  for (auto& msg: log) {

    char event = msg[0];

    std::string header;
    unsigned long num_messages(0);
    std::string messages;

    if (event == 'b') {
      
      header = event;
      num_messages = stoul(msg.substr(1, 2));
      messages = msg.substr(3, msg.size());

    } else if (event == 'd') {

      header = msg.substr(0, 7);
      num_messages = stoul(msg.substr(7, 2));
      messages = msg.substr(9, msg.size());

    } else {

        std::cerr << "Unknown log message " << msg << std::endl;
    }

    unsigned long msg_start(0);
    unsigned long msg_end(0);

    for (unsigned long i = 0; i < num_messages; ++i) {

      msg_end = messages.find('s', msg_start + 1);

      if (msg_end == std::string::npos) {
        msg_end = messages.size();
      }

      std::string message = header + messages.substr(msg_start, msg_end - msg_start); // assumes that no more than 9999 messages are sent

      messages_deconcat.push_back(message);

      msg_start = msg_end + 1;
    }
  }

  return messages_deconcat;
}

void writeOutput() {

  std::stringstream output;

  if (this_process != nullptr){

    std::list<std::string> log = deconcatMessages(this_process->getMessageLog());
    
    for (auto& message: log) {

      char event = message[0];

      if (event == 'd') {

        int msg_process_id = stoi(message.substr(1, 3));
        int msg_target_id = stoi(message.substr(4, 3));
        std::string payload = message.substr(7, message.size());

        output << "d " << msg_process_id << " " << payload << std::endl;

      } else if (event == 'b') {

        std::string payload = message.substr(1, message.size());

        output << "b " << payload << std::endl;

      } else {
        std::cerr << "Unknown log message " << message << std::endl;
      }
    }
  }

  std::ofstream file;

  file.open(outputPath);
  file << output.str(); 
  file.close();
}


static void stop(int) {
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  stopProcesses();

  // write/flush output file if necessary
  std::cout << "Writing output.\n";

  writeOutput();

  // exit directly from signal handler
  exit(0);
}

int main(int argc, char **argv) {
  signal(SIGTERM, stop);
  signal(SIGINT, stop);

  // `true` means that a config file is required.
  // Call with `false` if no config file is necessary.
  bool requireConfig = true;

  Parser parser(argc, argv);
  parser.parse();

  hello();
  std::cout << std::endl;

  std::cout << "My PID: " << getpid() << "\n";
  std::cout << "From a new terminal type `kill -SIGINT " << getpid() << "` or `kill -SIGTERM "
            << getpid() << "` to stop processing packets\n\n";

  std::cout << "My ID: " << parser.id() << "\n\n";

  std::cout << "List of resolved hosts is:\n";
  std::cout << "==========================\n";

  auto hosts = parser.hosts();

  for (auto &host : hosts) {

    std::cout << host.id << "\n";
    std::cout << "Human-readable IP: " << host.ipReadable() << "\n";
    std::cout << "Machine-readable IP: " << host.ip << "\n";
    std::cout << "Human-readbale Port: " << host.portReadable() << "\n";
    std::cout << "Machine-readbale Port: " << host.port << "\n";
    std::cout << "\n";

  }

  std::cout << "\n";

  std::cout << "Path to output:\n";
  std::cout << "===============\n";
  std::cout << parser.outputPath() << "\n\n";

  std::cout << "Path to config:\n";
  std::cout << "===============\n";
  std::cout << parser.configPath() << "\n\n";

  std::cout << "Doing some initialization...\n\n";

  outputPath = parser.outputPath();
  unsigned long my_id = parser.id();

  std::cout << "Number of hosts " << hosts.size() << std::endl;

  this_process = new Receiver(hosts[my_id-1].ip, hosts[my_id-1].port, static_cast<int>(my_id));
  beb = new BestEffortBroadcast(this_process);
  urb = new UniformReliableBroadcast(this_process, beb, hosts.size());

  // Initialize perfect links
  for (auto& host : hosts) {

    Sender* sender = nullptr;
    Perfectlink* link = nullptr;
    
    // Don't add a link to itself
    if (host.id != my_id) {
      sender = new Sender(createAddress(host.ip, host.port), static_cast<int>(host.id));
      link = new Perfectlink(this_process, sender, beb);
    }

    senders.push_back(sender);
    links.push_back(link);
  }

  for (auto& link: links) {
    if (link != nullptr) {
      link->addOtherLinks(links);
    }
  }

  beb->addLinks(links);  // lowest layer using the links

  // Read config file
  std::string m;
  std::string id;
  std::string line;
  std::ifstream readFile(parser.configPath());

  std::list<int> localized_ids; //dependencies

  // Get number of messages to send
  getline(readFile,line);
  std::stringstream iss(line);
  getline(iss, m, ' ');

  while(getline(readFile,line))   {
      std::stringstream iss(line);
      getline(iss, id, ' ');

      if (std::to_string(my_id) == id) {
        std::cout << "my id " << id << std::endl;

        std::string other_id;

        while(getline(iss, other_id, ' ')) {
          std::cout << "other id " << other_id << std::endl;
          localized_ids.push_back(stoi(other_id));
        }
      }
  }
  readFile.close();

  // Create broadcast
  broadcast = new LocalizedCausalBroadcast(this_process, urb, hosts.size(), localized_ids, bool (true));

  broadcast->setBroadcastActive();
  
  std::cout << "Broadcasting and delivering messages...\n\n";

  unsigned long num_messages_to_send = static_cast<unsigned long>(std::stoi(m));

  unsigned long groupy_by = 15; //group messages by 15

  std::string messages_concat = "";

  unsigned long msg_num(0); // nbr of messages concatenated

  for(unsigned int seq_num = 1; seq_num <= num_messages_to_send; ++seq_num){

    if (msg_num < groupy_by) {

      messages_concat += std::to_string(seq_num) + 's';
      msg_num += 1;
    }

    if (msg_num == groupy_by) {

      broadcast->broadcastMessage(std::to_string(msg_num) + messages_concat.substr(0, messages_concat.size() - 1)); //don't send last s
      msg_num = 0;
      messages_concat = "";
    }
  }

  char str_msg_num[3];
  sprintf(str_msg_num, "%02lu", msg_num);

  // the number of message to send wasn't a multiple of 15 -> send last messages concatenated
  if (messages_concat != "") {
    broadcast->broadcastMessage(std::string (str_msg_num) + messages_concat.substr(0, messages_concat.size() - 1));
  }

  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
