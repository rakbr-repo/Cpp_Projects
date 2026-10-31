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
        return handleSetConfig(payload);
    }

    return {UNKNOWN_COMMAND, "invalid command"};
}

PacketProcessingResult handlePing()
{
    return {SUCCESS, "PONG"};
}

PacketProcessingResult handleGetStatus()
{
    return {SUCCESS, "STATUS:RUNNING"};
}

PacketProcessingResult handleSetConfig(std::string& config)
{
    //To be implemented
}