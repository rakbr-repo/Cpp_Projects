#include <iostream>
#include <memory>
#include "include/network/tcp_server.hpp"
#include "include/network/tcp_server.hpp"
#include "include/network/connection.hpp"

int main()
{
    std::unique_ptr<TcpServer> tcpServer1 = std::make_unique<TcpServer>();
    if(tcpServer1->initializeServerSocket()!=0)
    {
        std::cout<<"Failed to init tcp server\n";
        return 1;
    }
    tcpServer1->run();
    return 0;
}