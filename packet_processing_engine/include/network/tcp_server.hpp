#pragma once
#include "connection.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unordered_map>


class TcpServer
{
    private:
    int serverSocket{-1};
    sockaddr_in serverAddr{};
    int epollFd{-1};
    std::unique_ptr<ConnectionHandler> connHandler;
    std::unordered_map<int, std::string> receiveBuffers;

    bool setNonBlocking(int fd);
    bool addToEpoll(int fd, std::uint32_t events);
    void acceptNewClients();
    void closeClient(int clientSocket);
    
    public:
    TcpServer();
    int initializeServerSocket();
    void run();
    void closeServer();
};