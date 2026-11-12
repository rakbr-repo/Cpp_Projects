#include "network/tcp_server.hpp"
#include <asm-generic/socket.h>
#include <cerrno>
#include <chrono>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

TcpServer::TcpServer(BoundedQueue<Message>& requestQueue, BoundedQueue<Message>& responseQueue,int& responseEventFd) : 
connHandler(std::make_unique<ConnectionHandler>()) , requestQueue(requestQueue), responseQueue(responseQueue), 
responseEventFd(responseEventFd) , running(true)
{}

bool TcpServer::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

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

    if(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        std::cerr<<"Epoll ctl add err : "<<std::strerror(errno) <<"\n";
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

    responseEventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    std::cout<<"Response fd tcpserver : "<<responseEventFd<<std::endl;
    if(responseEventFd == -1)
    {
        perror("eventfd\n");
        return -1;
    }
    if(!addToEpoll(responseEventFd, EPOLLIN))
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
            std::cerr<<"Client socket accept4 error : "<<std::strerror(errno)<<"\n";
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
    std::cout<<"Client Disconnected\n"<<std::endl;
}

void TcpServer::readClientData(int clientSocket)
{
    char buffer[4096];
    while(true)
    {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesRead > 0)
        {
            auto it = receiveBuffers.find(clientSocket);
            if(it == receiveBuffers.end())
            {
                closeClient(clientSocket);
                return;
            }

            std::string pending = it->second;
            pending.append(buffer, static_cast<std::size_t>(bytesRead));
            std::cout<<"Pending buffer after emplace : "<<pending<<std::endl;
            std::cout<<"Bytes Read : "<<bytesRead<<std::endl;

            std::size_t newLinePos;
            while((newLinePos = pending.find('\n')) != std::string::npos)
            {
                std::string message = pending.substr(0, newLinePos);
                if(!message.empty() && message.back() == '\r')
                {
                    message.pop_back();
                }
                pending.erase(0, newLinePos+1);
                Message reqMsg;
                reqMsg.clientSocket = clientSocket;
                std::cout<<"Message pushed : "<<message<<std::endl;
                reqMsg.payload = std::move(message);
                reqMsg.timeStamp = std::chrono::steady_clock::now();
                requestQueue.push(std::move(reqMsg));
                // connHandler->handleMessage(clientSocket, message);
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
            std::cout<<"EINTR "<<std::endl;
            continue;
        }
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
            std::cout<<"Err egain/ewouldblock "<<std::endl;
            return;
        }
        std::cerr<<" rcv read client data err : "<<std::strerror(errno)<<"\n";
        closeClient(clientSocket);
        return;
    }
}

void TcpServer::handleResponseEvent()
{
    std::uint64_t counter;
    ssize_t bytes = read(responseEventFd, &counter, sizeof(counter));
    if (bytes == sizeof(counter))
    {
        std::cout << "Worker notifications received: "
                  << counter << std::endl;
    }
    else if (bytes == -1 && errno != EAGAIN)
    {
        perror("eventfd read");
        return;
    }

    while(auto response = responseQueue.popTillEmpty())
    {
        size_t totalBytes = response->payload.size();
        size_t bytesSent = 0;
        while(bytesSent < totalBytes)
        {
            ssize_t bytes = send(response->clientSocket, response->payload.data() + bytesSent, totalBytes-bytesSent, 0);
            if(bytes > 0)
            {
                bytesSent+=bytes;
                std::cout<<"Response bytes sent: "<<bytesSent<<std::endl;
                continue;
            }
            else if(bytes == -1 && errno == EINTR)
            {
                continue;
            }
            else if (bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            {
                // Preserve unsent data and wait for EPOLLOUT.
                break;
            }
            else
            {
                // Handle socket error.
                perror("Socket error\n");
                break;
            }
        }
        std::cout<<"Response data : "<<response->payload<<std::endl;
    }
}

void TcpServer::run()
{
    if(serverSocket == -1 || epollFd == -1 || responseEventFd == -1)
    {
        std::cerr<<"Initialize before run call\n"<<"\n";
        return;
    }

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
            std::cerr<<"epoll wait : "<<std::strerror(errno)<<"\n";
            break;
        }

        for(int i=0; i < eventCount; i++)
        {
            int fd = events[i].data.fd;
            std::uint32_t flags = events[i].events;

            if(fd == serverSocket)
            {
                if(flags & EPOLLIN)
                {
                    acceptNewClients();
                }
                continue;
            }

            if(fd == responseEventFd)
            {
                if(flags & EPOLLIN)
                {
                    handleResponseEvent();
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

    if(responseEventFd != -1)
    {
        close(responseEventFd);
        responseEventFd = -1;
    }
}

void TcpServer::testConsumer(BoundedQueue<Message>& responseQueue)
{
    while(true)
    {
        auto msg = requestQueue.pop();

        if(msg)
        {
            std::cout<<"Message popped from consumer : "<<msg->payload<<std::endl;
            // Message response;
            // response.clientSocket = msg->clientSocket;
            // response.payload = msg->payload;

            // responseQueue.push(response);
        }
        else
        {
            std::cout<<"Consumer exiting\n"<<std::endl;
            return;
        }

    }
}