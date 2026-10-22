#pragma once
#include "connection.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>


class TcpServer
{
    private:
    int serverSocket{-1};
    sockaddr_in serverAddr{};
    int epollFd{-1};
    std::unique_ptr<ConnectionHandler> connHandler;

    bool setNonBlocking(int fd);
    bool addToEpoll(int fd, std::uint32_t events);
    public:
    TcpServer();
    int initializeServerSocket();
    void run();
    void closeServer();
};