#pragma once

#include <USBAPI.h>
#include <SoftwareSerial.h>
#include <Logger.hpp>
#include <Timer.hpp>


namespace mhz19b {
    template<class T, unsigned int N>
    class RingBuffer {
        T data[N];
        T* ptr;

        T* offset(unsigned int idx) const {
            T* result = ptr + idx;
            if (result - N >= data) {
                result -= N;
            }
            return result;
        }

    public:
        RingBuffer() : data{T()}, ptr(data) {}

        explicit RingBuffer(T initialValue) : data{initialValue}, ptr(data) {}

        unsigned int size() {
            return N;
        }

        T& operator[](unsigned int idx) const {
            return *offset(idx);
        }

        void add(T value) {
            *ptr = value;
            ptr = offset(1);
        }

        String toHex() const {
            char msg[N * 3];
            msg[sizeof(msg) - 1] = 0;
            char* cursor = msg;
            bool addSpace = false;
            for (uint8_t i = 0; i < N; ++i) {
                if (addSpace) {
                    *cursor = ' ';
                    ++cursor;
                }
                addSpace = true;
                uint8_t value = (*this)[i];
                uint8_t halfOfValue = value >> 4;
                if (halfOfValue < 10) {
                    *cursor = static_cast<char>((uint8_t) '0' + halfOfValue);
                } else {
                    *cursor = static_cast<char>((uint8_t) 'A' + (halfOfValue - 10));
                }
                ++cursor;
                halfOfValue = value & 0x0F;
                if (halfOfValue < 10) {
                    *cursor = static_cast<char>((uint8_t) '0' + halfOfValue);
                } else {
                    *cursor = static_cast<char>((uint8_t) 'A' + (halfOfValue - 10));
                }
                ++cursor;
            }
            return String(msg);
        }
    };

    class Supervisor {
        logger::Logger& logger;
        SoftwareSerial mhz19bSerial;
        Timer requestTimer;
        RingBuffer<uint8_t, 9> buffer;
        uint16_t co2ppm;

        uint8_t checksum(unsigned int offset) const {
            uint8_t result = 0;
            for (unsigned int i = offset; i < offset + 7; ++i) {
                result += buffer[i];
            }
            return 0xFF - result + 1;
        }

        uint8_t checksumForReading() const {
            return checksum(1);
        }

        uint8_t checksumForWriting() const {
            return checksum(2);
        }

    public:
        Supervisor(logger::Logger& logger, uint8_t receivePin, uint8_t transmitPin)
                : logger(logger),
                  mhz19bSerial(receivePin, transmitPin),
                  requestTimer(2000),
                  buffer(),
                  co2ppm{0} {
            mhz19bSerial.begin(9600);
            requestTimer.start();
        }

        uint16_t getCo2Ppm() {
            return co2ppm;
        }

        void loop() {
            if (mhz19bSerial.available()) {
                logger.debug() << "Time to receive data... " << mhz19bSerial.available() << " bytes available." << logger::end;
                do {
                    buffer.add(static_cast<uint8_t>(mhz19bSerial.read()));
                } while (mhz19bSerial.available() && !(buffer[0] == 0xFF && buffer[1] == 0x86));
                if (buffer[0] == 0xFF && buffer[1] == 0x86) {
                    if (checksumForReading() != buffer[8]) {
                        logger.warning()
                                << "Checksum does not match. Expected "
                                << checksumForReading()
                                << ", got"
                                << buffer[8]
                                << logger::end;
                    } else {
                        co2ppm = (static_cast<uint16_t>(buffer[2]) << 8) + buffer[3];
                    }
                } else {
                    logger.warning() << "Can't fetch co2 ppm. Buffer is " << buffer.toHex() << logger::end;
                }
                requestTimer.start();
            } else if (requestTimer.isReached()) {
                buffer.add(0xFF);  // 0 constant
                buffer.add(0x01);  // 1 sensor number, probably constant
                buffer.add(0x86);  // 2 read command
                buffer.add(0x00);  // 3
                buffer.add(0x00);  // 4
                buffer.add(0x00);  // 5
                buffer.add(0x00);  // 6
                buffer.add(0x00);  // 7
                buffer.add(checksumForWriting());  // 8
                for (unsigned int i = 0; i < buffer.size(); ++i) {
                    mhz19bSerial.write(buffer[i]);
                }
                requestTimer.stop();
                logger.debug() << "Sent buffer: " << buffer.toHex() << logger::end;
            }
        }
    };
}
