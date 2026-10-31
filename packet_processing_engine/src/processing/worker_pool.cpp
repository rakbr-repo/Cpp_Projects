#include "processing/worker_pool.hpp"

WorkerPool::WorkerPool(BoundedQueue<Message>& queue, std::size_t numberOfThreads) : 
bQueue(queue), numberOfThreads(numberOfThreads)
{
}

void WorkerPool::startWorker()
{
    for(int i=0;i<numberOfThreads;i++)
    {
        workers.emplace_back(&WorkerPool::workerPool, this);
    }
}

void WorkerPool::shutDown()
{
    for(int i=0;i<workers.size();i++)
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
        auto msg = bQueue.pop();
        if(!msg.has_value())
        {
            std::cout<<"Queue is empty exiting"<<std::endl;
            return;
        }
        PacketProcessor::PacketProcessingResult res = processor.process(msg);
        std::cout<<"Packet processed and status is : "<<res.status<<"Response is : "<<res.response<<std::endl;
    }
}