#pragma once
#include "connection.hpp"
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>


class TcpServer
{
    private:
    int serverSocket{-1};
    sockaddr_in serverAddr{};
    std::unique_ptr<ConnectionHandler> connHandler;
    public:
    TcpServer();
    int initializeServerSocket();
    void run();
    void closeServer();
};