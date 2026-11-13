#pragma once

#include "common/message.hpp"
#include "processing/packet_processor.hpp"
#include "queue/bounded_queue.hpp"
#include "common/message.hpp"
#include "metrics/metrics.hpp"
#include <thread>
#include <vector>
#include <unistd.h>

class WorkerPool
{
    private:
    BoundedQueue<Message>& requestQueue;
    BoundedQueue<Message>& responseQueue;
    int& responseEventFd;
    std::vector<std::thread> workers;
    PacketProcesser& processor;
    Metrics& metrics;
    std::size_t numberOfThreads;
    void workerPool();
    public:
    WorkerPool(BoundedQueue<Message>& requestQueue, BoundedQueue<Message>& responseQueue,
         PacketProcesser& processor,int& responseEventFd,Metrics& metrics,std::size_t numberOfThreads);

    void startWorkers();
    void shutDown();
};