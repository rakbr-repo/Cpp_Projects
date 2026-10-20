#pragma once
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>


class ConnectionHandler
{
    public:
    void handleClientRequests(int serverSocket);
    void closeServerConnection(int serverSocket);
};