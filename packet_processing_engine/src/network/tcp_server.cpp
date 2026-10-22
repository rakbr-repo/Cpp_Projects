#include "network/tcp_server.hpp"
#include <asm-generic/socket.h>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <sys/socket.h>
#include <memory>

TcpServer::TcpServer() : connHandler(std::make_unique<ConnectionHandler>())
{}

bool TcpServer::setNonBlocking(int fd)
{
    int flags = fnctl(fd, F_GETFL, 0);

    if(flags == -1)
    {
        perror("flag err");
        return false;
    }

    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("set flag err");
        return false;
    }

    return true;
}

bool TcpServer::addToEpoll(int fd , std::uint32_t events)
{
    epoll_event event{};
    event.events = events;
    event.data.fd = fd;

    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &events) == -1)
    {
        std:cerr<<"Epoll ctl add err : "<<stderr(errno) <<"\n";
        return false;
    }
    return true;
}

int TcpServer::initializeServerSocket()
{
    //create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == -1)
    {
        perror("failed to create server socket");
        return -1;
    }

    if(!setNonBlocking(serverSocket))
    {
        return -1;
    }

    int reuse = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        perror("setsockopt failed");
        close(serverSocket);
        serverSocket = -1;
        return -1;
    }
    //assign server address config
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    //bind call , assigns specific local Ip address and port no, to socket descriptor
    if(bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
    {
        perror("bind failed");
        close(serverSocket);
        serverSocket = -1;
        return -1;
    }

    //listen to the messages from clients
    if(listen(serverSocket, 5) == -1)
    {
        perror("listen failed");
        close(serverSocket);
        serverSocket = -1;
        return -1;
    }

    epollFd = epoll_create1(EPOLL_CLOEXEC);
    if(epollFd == -1)
    {
        perror("epoll create failed\n");
        return -1;
    }

    if(!addToEpoll(serverSocket, EPOLLIN))
    {
        return -1;
    }

    std::cout<<"Server socket created and the fd and its listening: "<<serverSocket<<std::endl;
    return serverSocket;
}

void TcpServer::run()
{
    if(serverSocket<0)
    {
        return;
    }
    connHandler->handleClientRequests(serverSocket);
}

void TcpServer::closeServer()
{
    if(serverSocket<0)
    {
        return;
    }
    connHandler->closeServerConnection(serverSocket);
}

