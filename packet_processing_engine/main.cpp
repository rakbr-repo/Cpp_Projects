#include <iostream>
#include <memory>
#include <thread>
#include <functional>
#include "include/network/tcp_server.hpp"
#include "include/common/message.hpp"
#include "include/network/connection.hpp"
#include "include/processing/worker_pool.hpp"
#include "include/processing/packet_processor.hpp"
#include "include/queue/bounded_queue.hpp"

int main()
{
    BoundedQueue<Message> responseQueue(100);
    BoundedQueue<Message> requestQueue(100);
    int responseEventFd{-1};
    std::unique_ptr<TcpServer> tcpServer = std::make_unique<TcpServer>(requestQueue, responseQueue,responseEventFd);
    std::unique_ptr<PacketProcesser> packetProcesser = std::make_unique<PacketProcesser>();
    
    // std::thread consumerThread(&TcpServer::testConsumer, tcpServer.get(), std::ref(requestQueue));
    if(tcpServer->initializeServerSocket() == -1)
    {
        std::cout<<"Failed to init tcp server\n";
        return 1;
    }
    std::unique_ptr<WorkerPool> workerPool = std::make_unique<WorkerPool>(requestQueue, responseQueue,
        *packetProcesser.get(),responseEventFd, static_cast<std::size_t>(3));
    
        
    workerPool->startWorkers();
    tcpServer->run();



    requestQueue.shutdown();
    workerPool->shutDown();
    responseQueue.shutdown();
    // consumerThread.join();
    tcpServer->closeServer();



    return 0;
}