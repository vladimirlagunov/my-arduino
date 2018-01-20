#include <ArduinoPlug.hpp>
#include <USBAPI.h>
#include <SerialLogger.hpp>
#include <SoftwareSerial.h>


const uint8_t MHZ19B_SERIAL_RECEIVE_PIN = 8;
const uint8_t MHZ19B_SERIAL_TRANSMIT_PIN = 9;


class MHZ19BExample {
    static const unsigned long durationMillis = 1000;

    SerialLogger logger;
    SoftwareSerial mhz19bSerial;
    bool requestSent;  // probably excess entity
    unsigned long lastResponseMillis;
    uint8_t buffer[9];
    uint8_t* bufferCycle;

    template<class T>
    bool isReached(T now, T previous, T addToPrevious) const {
        const T biggestByte = 1 << (sizeof(T) - 1);
        if ((previous & biggestByte) && !((previous + addToPrevious) & biggestByte)) {
            // overflow
            if (now & biggestByte) {
                // not overflowed yet
                return false;
            }
        }
        return previous + addToPrevious <= now;
    }

    unsigned char checksumForSending() const {
        unsigned char result = 0;
        for (unsigned char i = 1; i < 8; ++i) {
            result += buffer[i];
        }
        return static_cast<unsigned char>(0xFF - result + 1);
    }

    unsigned char checksumForReceiving() const {
        unsigned char result = 0;
        uint8_t* ptr = bufferCycle;
        int i = 1;
        while (i < 8) {
            result += *ptr;
            ptr = nextBufferCycle(ptr, 1);
            ++i;
        }
        return static_cast<unsigned char>(0xFF - result);  // no +1 - bug in scheme?
    }

    void logBuffer() {
        char msg[sizeof(buffer) * 3];
        msg[sizeof(msg) - 1] = 0;
        char* cursor = msg;
        uint8_t* src = bufferCycle;
        bool addSpace = false;
        for (uint8_t cnt = 0; cnt < sizeof(buffer); ++cnt) {
            if (addSpace) {
                *cursor = ' ';
                ++cursor;
            }
            addSpace = true;
            uint8_t i = *src;
            src = nextBufferCycle(src, 1);
            uint8_t half = i >> 4;
            if (half < 10) {
                *cursor = static_cast<char>((uint8_t) '0' + half);
            } else {
                *cursor = static_cast<char>((uint8_t) 'A' + (half - 10));
            }
            ++cursor;
            half = i & 0x0F;
            if (half < 10) {
                *cursor = static_cast<char>((uint8_t) '0' + half);
            } else {
                *cursor = static_cast<char>((uint8_t) 'A' + (half - 10));
            }
            ++cursor;
        }
        logger.debug(msg);
    }

    uint8_t* nextBufferCycle(uint8_t* ptr, uint8_t count) const {
        uint8_t* result = ptr + count;
        if (result >= buffer + sizeof(buffer)) {
            result -= sizeof(buffer);
        }
        return result;
    }

    uint8_t* nextBufferCycle(uint8_t count) const {
        return nextBufferCycle(bufferCycle, count);
    }

public:
    explicit MHZ19BExample()
            : logger(9600, Level::INFO), mhz19bSerial(MHZ19B_SERIAL_RECEIVE_PIN, MHZ19B_SERIAL_TRANSMIT_PIN), requestSent{false}, lastResponseMillis{0}, buffer{0}, bufferCycle{buffer} {
        mhz19bSerial.begin(9600);
        delay(1000);
        logger.info("Started up");
    }

    void loop() {
        if (mhz19bSerial.available()) {
            logger.debug("Time to receive data...");
            while (mhz19bSerial.available()) {
                *bufferCycle = static_cast<uint8_t>(mhz19bSerial.read());
                bufferCycle = nextBufferCycle(1);
                if (bufferCycle == buffer + sizeof(buffer)) {
                    bufferCycle = buffer;
                }
                if (*bufferCycle == 0xFF && *nextBufferCycle(1) == 0x86) {
                    logger.debug("Received:");
                    logBuffer();
                    if (checksumForReceiving() != buffer[8]) {
                        logger.debug(("Checksum does not match. Expected " + String(checksumForReceiving())
                                      + ", got " + String(buffer[8])).c_str());
                    } else {
                        uint16_t ppm = (static_cast<uint16_t>(buffer[2]) << 8) + buffer[3];
                        logger.info(("PPM: " + String(ppm)).c_str());
                    }
                    lastResponseMillis = millis();
                    logger.debug(("Next expected check at " + String(lastResponseMillis + durationMillis)).c_str());
                    requestSent = false;
                    break;
                }
            }
        } else if (isReached(millis(), lastResponseMillis, durationMillis)) {
            logger.debug("Time to send data...");
            bufferCycle = buffer;
            buffer[0] = 0xFF;  // constant
            buffer[1] = 0x01;  // sensor number, probably constant
            buffer[2] = 0x86;  // read command
            buffer[3] = 0x00;
            buffer[4] = 0x00;
            buffer[5] = 0x00;
            buffer[6] = 0x00;
            buffer[7] = 0x00;
            buffer[8] = checksumForSending();
            for (uint8_t i : buffer) {
                mhz19bSerial.write(i);
            }
            requestSent = true;
            logger.debug("Sent:");
            logBuffer();
        }
        delay(100);
    }
};

ArduinoPlug<MHZ19BExample> plug;

void setup() {
    plug.setup();
}

void loop() {
    plug.loop();
}