#pragma once

enum class Level {
    DEBUG = 1,
    INFO = 2,
};

class SerialLogger {
    Level level;
    void sendMsg(const char* logLevel, const char* msg);
public:
    SerialLogger(uint32_t baudRate, Level level);
    void info(const char* msg);
    void debug(const char* msg);
};
