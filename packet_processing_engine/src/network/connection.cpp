#include "network/connection.hpp"
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>

void ConnectionHandler::handleMessage(int clientSocket, const std::string& message)
{
    std::cout<<"Client socket fd : "<<clientSocket<<" message is"<<message<<std::endl;
}
