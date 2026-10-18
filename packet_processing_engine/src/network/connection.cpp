#include "network/connection.hpp"
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>

void connectionHandler::handleClientRequests(int serverSocket)
{
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

    if(clientSocket==-1)
    {
        perror("accept socket call failed\n");
        return;
    }

    char buffer[1025];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);

    if(bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        std::cout << "Message from client: " << buffer
                  << std::endl;
    }
    else if(bytesReceived == 0)
    {
        std::cout<<"Client disconnected\n"<<std::endl;
    }
    else
    {
        perror("recv call failed\n");
    }

    close(clientSocket);
}

void connectionHandler::closeServerConnection(int serverSocket)
{
    if(serverSocket >= 0)
    {
        close(serverSocket);
    }
}