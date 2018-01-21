#include <USBAPI.h>
#include "Timer.hpp"

Timer::Timer(unsigned long duration) : duration(duration), lastEventMillis(0) {}

void Timer::start() {
    active = true;
    lastEventMillis = millis();
}

bool Timer::isReached() const {
    if (!active) {
        return false;
    }
    unsigned long now = millis();
    if ((lastEventMillis & biggestByte)
        && !((lastEventMillis + duration) & biggestByte)  // next timestamp would be overflown
        && now & biggestByte) {  // current timestamp not overflown yet
        return false;
    } else {
        return lastEventMillis + duration <= now;
    }
}

unsigned long Timer::willBeReachedAt() const {
    return lastEventMillis + duration;
}

void Timer::stop() {
    active = false;
}

bool Timer::isActive() const {
    return active;
}
