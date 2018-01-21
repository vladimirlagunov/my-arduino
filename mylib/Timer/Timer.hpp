#pragma once

class Timer {
    static const unsigned long biggestByte = 1 << (sizeof(unsigned long) - 1);
    const unsigned long duration;
    unsigned long lastEventMillis;
    bool active;
public:
    explicit Timer(unsigned long duration);
    void start();
    void stop();
    bool isReached() const;
    bool isActive() const;
    unsigned long willBeReachedAt() const;
};
