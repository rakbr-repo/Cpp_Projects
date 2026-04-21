#include <CANReader.hpp>
using namespace CanToCloud::Can;

bool CANReader::initializeCANReader()
{
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("socket");
        return false;
    }

    // 2. Specify the CAN interface (vcan0)
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // 3. Bind the socket to vcan0
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1false;
    }
    std::cout<<"Socket and Interface init successfull";
    return true;
}

void CANReader::registerObserver(CANObserver* observer)
{
    observers.push_back(observer);
}

void CANReader::removeObserver(CANObserver* observer)
{
    observers.erase(std::remove(observers.begin(),observers.end(),observers), oberservers.end());
}

void CANReader::notifyObservers()
{
    for(auto obs : observers)
    {
        obs->update(frame);
    }
}

void CANReader::readCANFrames() {
    while (true) 
    {
        int nbytes = read(sock, &frame, sizeof(struct can_frame));
        if (nbytes < 0) 
        {
            perror("read");
            break;
        }
        if (nbytes < sizeof(struct can_frame)) 
        {
            std::cerr << "Incomplete CAN frame\n";
            continue;
        }
        notifyObservers();
    }
}
