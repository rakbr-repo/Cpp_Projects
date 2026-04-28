#pragma once
#include <string>
#include <linux/can.h>
#include <nlohmann/json.hpp>
#include "HttpClient.hpp"
#include "CanObInterface.hpp"

namespace CanToCloud {
namespace Json {

class JsonBuilder : public CANObserver {
private:
    nlohmann::json j;

public:
    JsonBuilder& addCANFrame(const struct can_frame& frame);
    JsonBuilder& addTimestamp();
    JsonBuilder& addMetadata(const std::string& key, const std::string& value);

    std::string build() const;
    void update(struct can_frame frame) override;
};

} // namespace Json
} // namespace CanToCloud
