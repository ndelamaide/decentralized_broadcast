# DO NAME THE SYMBOLIC VARIABLE `SOURCES`

include_directories(include)
set(SOURCES src/main.cpp src/hello.c src/udpserver.cpp src/udpclient.cpp src/containers/threadsafequeue.cpp
     src/containers/threadsafelist.cpp src/receiver.cpp src/sender.cpp src/perfectlink.cpp
     src/broadcast/beb.cpp)

# DO NOT EDIT THE FOLLOWING LINE
find_package(Threads)
add_executable(da_proc ${SOURCES})
target_link_libraries(da_proc ${CMAKE_THREAD_LIBS_INIT})
