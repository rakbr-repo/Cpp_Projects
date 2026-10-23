#include "network/tcp_server.hpp"
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>

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

void TcpServer::acceptNewClients()
{
    while(true)
    {
        int clientSocket = accept4(serverSocket, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if(clientSocket == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            std::cerr<<"Client socket accept4 error : "<<stderr(errno)<<"\n";
            break;
        }

        if(!addToEpoll(clientSocket, EPOLLIN | EPOLLRDHUP))
        {
            close(clientSocket);
            continue;
        }
        receiveBuffers.emplace(clientSocket, std::string{});
        std::cout<<"Accepted new client : "<<clientSocket<<std::endl;
    }
}

void TcpServer::closeClient(int clientSocket)
{
    epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, nullptr);
    close(clientSocket);
    receiveBuffers.erase(clientSocket);
    std::cout<<"Client Disconnected\n"<<std:endl;
}

void TcpServer::readClientData(int clientSocket)
{
    char buffer[4096];
    while(true)
    {
        std::ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesRead > 0)
        {
            auto it = receiveBuffers.find(clientSocket);
            if(it == receiveBuffers.end())
            {
                closeClient(clientSocket);
                return;
            }

            std::string pending = it->second;
            pending.emplace(buffer, static_cast<std::size_t>(bytesRead));

            std::size_t newLinePos;
            while((newLinePos = pending.find('\n')) != std::npos)
            {
                std::string message = pending.substr(0, newLinePos);
                if(!message.empty() && message.back() == '\r')
                {
                    message.pop_back();
                }
                pending.erase(0, newLinePos+1);
                connHandler->handleMessage(clientSocket, message);
            }
            continue;
        }

        if(bytesRead == 0)
        {
            closeClient(clientSocket);
            return;
        }

        if(errno == EINTR)
        {
            continue;
        }
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            closeClient(clientSocket);
            return;
        }
        std::cerr<<" rcv read client data err : "<<stderr(errno)<<"\n";
        closeClient(clientSocket);
        return;
    }
}

void TcpServer::run()
{
    if(serverSocket == -1 && epollFd == -1)
    {
        std::cerr<<"Initialize before run call\n"<<"\n";
        return;
    }

    bool running = true;
    epoll_event events[64];

    while(running)
    {
        int eventCount = epoll_wait(epollFd, events,64, -1);

        if(eventCount == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            std::cerr<<"epoll wait : "<<stderr(errno)<<"\n";
            break;
        }

        for(int i=0; i < eventCount; i++)
        {
            int fd = events[i].data.fd;
            std::unint32_t flags = events[i].events;

            if(fd == serverSocket)
            {
                if(flags & EPOLLIN)
                {
                    acceptNewClients();
                }
                continue;
            }

            if(flags & EPOLLIN)
            {
                readClientData(fd);
            }

            if((receiveBuffers.find(fd)!=receiveBuffers.end()) && (flags & (EPOLLERR | EPOLLHUP |EPOLLRDHUP)))
            {
                closeClient(fd);
            }
        }
    }
}

void TcpServer::closeServer()
{
    running = false;
    for(const auto& [fd, message] : receiveBuffers)
    {
        if(fd != -1)
        {
            if(epollFd != -1)
            {
                epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
            }
            close(fd);
        }
    }
    receiveBuffers.clear();
    if(serverSocket != -1)
    {
        if(epollFd != -1)
        {
            epoll_ctl(epollFd, EPOLL_CTL_DEL, serverSocket, nullptr);
        }
        close(serverSocket);
        serverSocket = -1;
    }

    if(epollFd != -1)
    {
        close(epollFd);
        epollFd = -1;
    }
}

