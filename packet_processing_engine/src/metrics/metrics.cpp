#include "metrics/metrics.hpp"
#include <iostream>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <mutex>
#include<algorithm>

void Metrics::recordLatency(std::chrono::microseconds sec)
{
    std::lock_guard<std::mutex> lck(latencyMtx);
    latencySamples.push_back(static_cast<double>(sec.count()));
}

void Metrics::percentileCalculation()
{
    if(latencySamples.size()==0)
    {
        p50 =0;p90=0;p99=0;
        return;
    }
    std::lock_guard<std::mutex> lck(latencyMtx);
    std::sort(latencySamples.begin(), latencySamples.end());

    std::size_t n = latencySamples.size();

    std::size_t p50Idx = std::min(static_cast<size_t>(0.5*n),n-1);
    std::size_t p90Idx = std::min(static_cast<size_t>(0.9*n),n-1);
    std::size_t p99Idx = std::min(static_cast<size_t>(0.99*n),n-1);
    p50 = latencySamples[p50Idx];
    p90 = latencySamples[p90Idx];
    p99 = latencySamples[p99Idx];
}

void Metrics::startTimeForThroughput()
{
    startTime = std::chrono::steady_clock::now();
}

void Metrics::calculateAndPrintThroughput()
{
    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    double elapsedSeconds = std::chrono::duration<double>(endTime-startTime).count();

    double throughPut = processedMessages.load()/elapsedSeconds;
    std::cout<<"ThroughPut value : "<<throughPut<<std::endl;
}


void Metrics::printStatistics()
{
    std::cout<<"--------STATISTICS----------"<<std::endl;
    std::cout<<"Processed Messages : "<<processedMessages<<std::endl;
    std::cout<<"Received Messages : "<<receivedMessages<<std::endl;
    std::cout<<"Rejected Messages : "<<rejectMessages<<std::endl;
    std::cout<<"Failed Messages : "<<failedMessages<<std::endl;

    calculateAndPrintThroughput();
    percentileCalculation();
    std::cout<<"Percenticle statistics : "<<std::endl;
    std::cout<<"p50 : "<<p50<<std::endl;
    std::cout<<"p90 : "<<p90<<std::endl;
    std::cout<<"p99 : "<<p99<<std::endl;


}

void Metrics::incrementProcessedMessage()
{
    processedMessages.fetch_add(1,std::memory_order_relaxed);
}

void Metrics::incrementFailedMessage()
{
    failedMessages.fetch_add(1,std::memory_order_relaxed);;
}

void Metrics::incrementReceivedMessages()
{
    receivedMessages.fetch_add(1,std::memory_order_relaxed);;
}

void Metrics::incrementRejectMessages()
{
    rejectMessages.fetch_add(1,std::memory_order_relaxed);;
}
