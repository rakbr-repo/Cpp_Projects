#include "metrics/metrics.hpp"
#include <chrono>
#include <mutex>

void Metrics::recordLatency(std::chrono::microseconds sec)
{
    std::lock_guard<std::mutex> lck(LatencyMtx);
    latencySamples.push_back(static_cast<double>(sec));
}

void Metrics::incrementProcessedMessage()
{
    processedMessages++;
}

void Metrics::incrementFailedMessage()
{
    failedMessages++;
}

void Metrics::incrementReceivedMessages()
{
    receivedMessages++;
}

void Metrics::incrementRejectMessages()
{
    rejectMessages++;
}
