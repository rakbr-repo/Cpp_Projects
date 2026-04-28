#include "JsonBuilder.hpp"
#include <chrono>

using namespace CanToCloud::Json;
using namespace CanToCloud::http;

JsonBuilder& JsonBuilder::addCANFrame(const struct can_frame& frame) {
    j["can_id"] = frame.can_id;
    j["dlc"] = frame.can_dlc;

    std::vector<std::string> data;
    for (int i = 0; i < frame.can_dlc; i++) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%02X", frame.data[i]);
        data.push_back(std::string(buf));
    }
    j["data"] = data;

    return *this;
}

JsonBuilder& JsonBuilder::addTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()).count();
    j["timestamp"] = ms;
    return *this;
}

JsonBuilder& JsonBuilder::addMetadata(const std::string& key, const std::string& value) {
    j["metadata"][key] = value;
    return *this;
}

std::string JsonBuilder::build() const {
    return j.dump();
}

void JsonBuilder::update(struct can_frame frame)
{
    std::string url = "http://localhost:5000/data";
    std::string payload = addCANFrame(frame)
                              .addTimestamp()
                              .addMetadata("source", "CANReader")
                              .build();
    HttpClient::getInstance().publish(url, payload);
}
