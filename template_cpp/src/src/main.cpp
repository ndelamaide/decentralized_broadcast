#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include <sstream>

#include "parser.hpp"
#include "hello.h"
#include <signal.h>

#include "receiver.hpp"
#include "sender.hpp"
#include "perfectlink.hpp"

#define MAX_LENGTH 32

sockaddr_in createAddress(in_addr_t ip, unsigned short port);

Receiver* this_process = nullptr;
std::vector<Sender*> senders;
std::vector<Perfectlink*> links;


sockaddr_in createAddress(in_addr_t ip, unsigned short port) {
    
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = ip;
  address.sin_port = port;
  memset(address.sin_zero, '\0', sizeof(address.sin_zero));

  return address;
}


static void stop(int) {
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  // write/flush output file if necessary
  std::cout << "Writing output.\n";

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

  unsigned long my_id = parser.id();

  this_process = new Receiver(hosts[my_id-1].ip, hosts[my_id-1].port, static_cast<int>(my_id));

  // Initialize perfect links
  for (auto& host : hosts) {

    Sender* sender = nullptr;
    Perfectlink* link = nullptr;
    
    // Don't add a link to itself
    if (host.id != my_id) {
      sender = new Sender(createAddress(host.ip, host.port));
      link = new Perfectlink(this_process, sender);
    }

    senders.push_back(sender);
    links.push_back(link);
  }

  // Read config file
  std::string m, i;
  std::string line;
  std::ifstream readFile(parser.configPath());

  while(getline(readFile,line))   {
      std::stringstream iss(line);
      getline(iss, m, ' ');
      getline(iss, i, '\n');
  }
  readFile.close();

  unsigned long num_messages_to_send = static_cast<unsigned long>(std::stoi(m));
  unsigned long target_process_id = static_cast<unsigned long>(std::stoi(i));

  if (!(my_id == target_process_id)){

    for(unsigned int seq_num = 1; seq_num <= num_messages_to_send; ++seq_num){

      // Inside perfect link class ?
      char packet[MAX_LENGTH] = {0};
      int ack = 0;
      sprintf(packet, "%-1d%03lu%-d%-c", ack, my_id, seq_num, '\0');

      links[target_process_id-1]->addMessage(packet);
    }
  }
  
  std::cout << "Broadcasting and delivering messages...\n\n";

  for (auto& link: links) {
    if (link != nullptr) {
      link->setLinkActive();
    }
  }

  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  while (true) {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}