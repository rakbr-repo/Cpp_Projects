#include <iostream>
#include <memory>
#include "include/network/tcp_server.hpp"
#include "include/network/tcp_server.hpp"
#include "include/network/connection.hpp"

int main()
{
    std::unique_ptr<tcpServer> tcpServer1 = std::make_unique<tcpServer>();
    int tcpServerSocket = tcpServer1->initializeServerSocket();
    tcpServer1->handleClientRequests(tcpServerSocket);
    tcpServer1->closeServerConnection(tcpServerSocket);
}