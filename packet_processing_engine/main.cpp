#include <iostream>
#include <memory>
#include "include/network/tcp_server.hpp"
#include "include/network/tcp_server.hpp"
#include "include/network/connection.hpp"
#include "include/queue/bounded_queue.hpp"

int main()
{
    BoundedQueue<Message> responseQueue(100);
    std::unique_ptr<TcpServer> tcpServer1 = std::make_unique<TcpServer>(responseQueue);
    if(tcpServer1->initializeServerSocket()!=0)
    {
        std::cout<<"Failed to init tcp server\n";
        return 1;
    }
    tcpServer1->run();
    return 0;
}