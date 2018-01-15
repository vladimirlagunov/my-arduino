#include <HardwareSerial.h>
#include <USBAPI.h>
#include "SerialLogger.hpp"

SerialLogger::SerialLogger(uint32_t baudRate) {
    Serial.begin(baudRate);
}

void SerialLogger::info(const char* msg) {
    Serial.print('[');
    Serial.print(millis());
    Serial.print("] INFO ");
    Serial.println(msg);
}
