#pragma once
#include "connection.hpp"
#include "queue/bounded_queue.hpp"
#include "metrics/metrics.hpp"
#include "common/message.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unordered_map>
#include <cstring>

class TcpServer
{
    private:
    int serverSocket{-1};
    sockaddr_in serverAddr{};
    int epollFd{-1};
    int& responseEventFd;
    bool running;
    std::unique_ptr<ConnectionHandler> connHandler;
    std::unordered_map<int, std::string> receiveBuffers;
    BoundedQueue<Message>& responseQueue;
    BoundedQueue<Message>& requestQueue;
    Metrics& metrics;

    bool setNonBlocking(int fd);
    bool addToEpoll(int fd, std::uint32_t events);
    void acceptNewClients();
    void closeClient(int clientSocket);
    void readClientData(int clientSocket);
    void handleResponseEvent();
    
    public:
    TcpServer(BoundedQueue<Message>& requestQueue,BoundedQueue<Message>& responseQueue,Metrics& metrics,int& responseEventFd);
    int initializeServerSocket();
    void run();
    void closeServer();
};