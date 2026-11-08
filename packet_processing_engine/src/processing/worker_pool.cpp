#include "../../include/processing/worker_pool.hpp"
#include <cerrno>
#include <cstddef>
#include <iostream>
#include <unistd.h>
#include<iostream>

WorkerPool::WorkerPool(BoundedQueue<Message>& requestQueue, BoundedQueue<Message>& responseQueue, 
    PacketProcesser& processor,int& responseEventFd, std::size_t numberOfThreads) : 
requestQueue(requestQueue), responseQueue(responseQueue), processor(processor),responseEventFd(responseEventFd), numberOfThreads(numberOfThreads)
{
}

void WorkerPool::startWorkers()
{
    for(int i=0;i<static_cast<int>(numberOfThreads);i++)
    {
        workers.emplace_back(&WorkerPool::workerPool, this);
    }
}

void WorkerPool::shutDown()
{
    requestQueue.shutdown();
    for(int i=0;i<static_cast<int>(workers.size());i++)
    {
        if(workers[i].joinable())
        {
            workers[i].join();
        }
    }
}

void WorkerPool::workerPool()
{
    while(true)
    {
        auto msg = requestQueue.pop();
        if(!msg.has_value())
        {
            std::cout<<"Queue is empty exiting"<<std::endl;
            return;
        }
        PacketProcessingResult res = processor.process(*msg);
        std::cout<<"Packet processed and status is : "<<static_cast<int>(res.status)<<"Response is : "<<res.response<<std::endl;

        Message response;
        response.clientSocket = msg->clientSocket;
        response.payload = res.response;

        responseQueue.push(response);

        std::uint64_t value = 1;
        std::cout<<"Response fd : "<<responseEventFd<<std::endl;
        std::size_t bytesWritten = write(responseEventFd, &value, sizeof(value));
        std::cout<<"bytesWritten  : "<<static_cast<int>(bytesWritten)<<std::endl;

        if(bytesWritten == sizeof(value))
        {
            break;
        }

        if(bytesWritten == -1 && errno == EINTR)
        {
            continue;
        }
        else if(bytesWritten == -1 && (errno == EWOULDBLOCK || errno == EAGAIN))
        {
            break;
        }
        perror("eventfd write");
        break;
    }
}