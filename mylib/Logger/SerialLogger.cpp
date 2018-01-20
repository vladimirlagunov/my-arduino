#include <HardwareSerial.h>
#include <USBAPI.h>
#include "SerialLogger.hpp"

SerialLogger::SerialLogger(uint32_t baudRate, Level level) : level(level) {
    Serial.begin(baudRate);
}

void SerialLogger::info(const char* msg) {
    if (level <= Level::INFO) {
        sendMsg("INFO", msg);
    }
}

void SerialLogger::debug(const char* msg) {
    if (level <= Level::DEBUG) {
        sendMsg("DEBUG", msg);
    }
}

void SerialLogger::sendMsg(const char* logLevel, const char* msg) {
    Serial.print('[');
    Serial.print(millis());
    Serial.print("] ");
    Serial.print(logLevel);
    Serial.print(" ");
    Serial.println(msg);
}
