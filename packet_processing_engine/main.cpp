#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include "include/network/tcp_server.hpp"
#include "include/common/message.hpp"
#include "include/network/connection.hpp"
#include "include/queue/bounded_queue.hpp"

int main()
{
    BoundedQueue<Message> responseQueue(100);
    BoundedQueue<Message> requestQueue(100);
    std::unique_ptr<TcpServer> tcpServer = std::make_unique<TcpServer>(requestQueue, responseQueue);

    std::thread consumerThread(&TcpServer::testConsumer, tcpServer.get(), std::ref(requestQueue));
    if(tcpServer->initializeServerSocket() == -1)
    {
        std::cout<<"Failed to init tcp server\n";
        return 1;
    }
    tcpServer->run();

    requestQueue.shutdown();
    responseQueue.shutdown();
    consumerThread.join();
    tcpServer->closeServer();



    return 0;
}