#pragma once
#include <atomic>
#include <vector>
#include <chrono>
#include <mutex>

class Metrics
{
    private:
    std::atomic<std::uint64_t> receivedMessages{0};
    std::atomic<std::uint64_t> processedMessages{0};
    std::atomic<std::uint64_t> failedMessages{0};
    std::atomic<std::uint64_t> rejectMessages{0};
    
    std::mutex latencyMtx;
    std::vector<double> latencySamples;

    public:
    void incrementProcessedMessage();
    void incrementFailedMessage();
    void incrementReceivedMessages();
    void incrementRejectMessages();

    void recordLatency(std::chrono::microseconds sec);
};