#pragma once

#include "common/message.hpp"
#include "packet_processor.hpp"
#include "queue/bounded_queue.hpp"
#include <thread>
#include <vector>

class WorkerPool
{
    private:
    BoundedQueue<Message>& bQueue;
    std::vector<std::thread> workers;
    PacketProcessor& processor;
    std::size_t numberOfThreads;
    void workerLoop();
    public:
    WorkerPool(BoundedQueue<Message>& queue, int& numberOfThreads);

    void startWorker();
    void shutdown();
};