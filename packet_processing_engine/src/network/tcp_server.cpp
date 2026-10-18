#include "network/tcp_server.hpp"
#include <asm-generic/socket.h>
#include <cstdio>
#include <sys/socket.h>
#include <memory>

int tcpServer::initializeServerSocket()
{
    //create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == -1)
    {
        perror("failed to create server socket");
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

    std::cout<<"Server socket created and the fd and its listening: "<<serverSocket<<std::endl;
    return serverSocket;
}



