#pragma once

#include "common/message.hpp"
#include "processing/packet_processor.hpp"
#include "queue/bounded_queue.hpp"
#include "common/message.hpp"
#include <thread>
#include <vector>

class WorkerPool
{
    private:
    BoundedQueue<Message>& requestQueue;
    BoundedQueue<Message>& responseQueue;
    std::vector<std::thread> workers;
    PacketProcesser& processor;
    std::size_t numberOfThreads;
    void workerPool();
    public:
    WorkerPool(BoundedQueue<Message>& requestQueue, BoundedQueue<Message>& responseQueue, PacketProcesser& processor, std::size_t numberOfThreads);

    void startWorkers();
    void shutDown();
};