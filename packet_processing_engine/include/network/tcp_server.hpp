#pragma once
#include "connection.hpp"
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>


class tcpServer : public connectionHandler
{
    private:
    int serverSocket{-1};
    sockaddr_in serverAddr{};
    public:
    int initializeServerSocket();
};