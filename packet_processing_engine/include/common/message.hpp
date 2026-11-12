#pragma once
#include <chrono>
#include<string>

class Message
{
    public:
    int clientSocket{-1};
    std::string payload{};
    std::chrono::steady_clock::time_point timeStamp;
};