#pragma once
#include<string>

class Message
{
    public:
    int clientSocket{-1};
    std::string payload{};
};