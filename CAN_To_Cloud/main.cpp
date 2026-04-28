#include <csignal>
#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "CANReader.hpp"
#include "HttpClient.hpp"
#include "JsonBuilder.hpp"
// using CanToCloud::Can;
// using CanToCloud::http;
// using CanToCloud::JsonBuilder;

std::atomic<bool> running{true};
std::mutex mtx;
std::condition_variable cv;

void signalHandler(int signum) {
    std::cerr << "Caught signal " << signum << std::endl;
    running = false;
    cv.notify_all();
}


int main() 
{
    std::cout << "Start of Can To Cloud !" << std::endl;
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "Initialize Curl" <<std::endl;
    CanToCloud::http::HttpClient::getInstance().initializeCurl();
    CanToCloud::Can::CANReader reader;
    CanToCloud::Json::JsonBuilder builder;
    std::cout << "registerObserver -> Json Builder" <<std::endl;
    reader.registerObserver(&builder);

    if (reader.initializeCANReader()) {
        std::cout<<"Start reading CAN Frames"<<std::endl;
        reader.start();
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !running.load(); });
    }

    reader.stop();
    CanToCloud::http::HttpClient::getInstance().destroyCurl();
    std::cout << "Program terminated gracefully." << std::endl;
    return 0;
}

