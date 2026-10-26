#include <cstddef>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSocket == -1)
    {
        perror("client socket creation failed");
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) != 1)
    {
        std::cout << "Invalid server address\n";
        close(clientSocket);
        return 1;
    }

    if(connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1)
    {
        perror("connect call failed\n");
        close(clientSocket);
        return 1;
    }

    const char* message = "Hello! Server\n";
    size_t messageLength = strlen(message);
    size_t totalBytesSent = 0;

    while(totalBytesSent < messageLength)
    {
        ssize_t bytesSent = send(clientSocket, message + totalBytesSent, messageLength-totalBytesSent,0);

        if (bytesSent <= 0)
        {
            if (bytesSent == -1)
                perror("send");
            else
                std::cerr << "send returned zero\n";

            close(clientSocket);
            return 1;
        }
        totalBytesSent += static_cast<size_t>(bytesSent);
    }

    std::cout<<"Message sent successfully\n"<<std::endl;
    close(clientSocket);
    return 0;
}