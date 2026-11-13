#pragma once
#include <atomic>
#include <cstddef>
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

    std::size_t p50,p90,p99;
    std::chrono::steady_clock::time_point startTime;
    
    std::mutex latencyMtx;
    std::vector<double> latencySamples;
    void percentileCalculation();

    public:
    void incrementProcessedMessage();
    void incrementFailedMessage();
    void incrementReceivedMessages();
    void incrementRejectMessages();
    void calculateAndPrintThroughput();
    void startTimeForThroughput();
    void printStatistics();
    void recordLatency(std::chrono::microseconds sec);
};