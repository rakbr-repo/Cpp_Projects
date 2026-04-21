#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "CanObInterface.hpp"

namespace CanToCloud
{
namespace Can
{
class CANReader : public CANSubject
{
    private:
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    std::vector<CANObserver*> observers;

    public:
    bool initializeCANReader();
    void registerObserver(CANObserver* observer) override;
    void removeObserver(CANObserver* observer) override;
    void notifyObservers() override;

}
}
}