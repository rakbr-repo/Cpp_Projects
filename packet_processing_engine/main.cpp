#include <iostream>
#include <memory>
#include "include/network/tcp_server.hpp"
#include "include/network/tcp_server.hpp"
#include "include/network/connection.hpp"

int main()
{
    std::unique_ptr<TcpServer> tcpServer1 = std::make_unique<TcpServer>();
    tcpServer1->initializeServerSocket();
    tcpServer1->run();
    tcpServer1->closeServer();
}