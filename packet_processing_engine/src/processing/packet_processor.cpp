#include "processing/packet_processor.hpp"

PacketProcessingResult PacketProcesser::process(Message& message)
{
    if(message.payload == "PING")
    {
        return handlePing();
    }
    else if(message.payload == "STATUS")
    {
        return handleGetStatus();
    }
    else if(message.payload == "CONFIG")
    {
        return handleSetConfig(message.payload);
    }

    return {UNKNOWN_COMMAND, "invalid command"};
}

PacketProcessingResult PacketProcesser::handlePing()
{
    return {SUCCESS, "PONG"};
}

PacketProcessingResult PacketProcesser::handleGetStatus()
{
    return {SUCCESS, "STATUS:RUNNING"};
}

PacketProcessingResult PacketProcesser::handleSetConfig(std::string& config)
{
    //To be implemented
    PacketProcessingResult res;
    res.response = config;
    return res;
}