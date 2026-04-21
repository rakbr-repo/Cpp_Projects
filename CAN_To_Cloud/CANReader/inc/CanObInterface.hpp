#include <iostream>
#include <linux/can.h>

class CANObserver {
public:
    virtual void update(struct can_frame frame) = 0;
    virtual ~CANObserver() {}
};

class CANSubject {
public:
    virtual void registerObserver(CANObserver* observer) = 0;
    virtual void removeObserver(CANObserver* observer) = 0;
    virtual void notifyObservers() = 0;
    virtual ~CANSubject() {}
};