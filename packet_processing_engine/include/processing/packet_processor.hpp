#pragma once 

#include "common/message.hpp"
#include <string>

enum PacketProcessingStatus
{
    SUCCESS,
    INVALID_MESSAGE,
    UNKNOWN_COMMAND,
    PROCESSING_FAILURE
};

struct PacketProcessingResult
{
    PacketProcessingStatus status;
    std::string response;
};

class PacketProcesser
{
    public:
    PacketProcessingResult process(Message& message);

    private:
    PacketProcessingResult handlePing();
    PacketProcessingResult handleGetStatus();
    PacketProcessingResult handleSetConfig(std::string& config);
};