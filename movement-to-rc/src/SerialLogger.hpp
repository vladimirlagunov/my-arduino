#pragma once
#include <stdint.h>

class SerialLogger {
public:
    SerialLogger(uint32_t baudRate);
    void info(const char* msg);
};
