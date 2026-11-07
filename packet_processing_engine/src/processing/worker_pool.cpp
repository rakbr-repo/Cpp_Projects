#include "../../include/processing/worker_pool.hpp"
#include <iostream>

WorkerPool::WorkerPool(BoundedQueue<Message>& requestQueue, BoundedQueue<Message>& responseQueue, 
    PacketProcesser& processor, std::size_t numberOfThreads) : 
requestQueue(requestQueue), responseQueue(responseQueue), processor(processor), numberOfThreads(numberOfThreads)
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
    }
}