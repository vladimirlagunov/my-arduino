#pragma once

#include <HardwareSerial.h>
#include <USBAPI.h>
#include <Logger.hpp>

namespace logger {
    template<class Stream>
    class StreamLogger : public Logger {
        static void writeMessage(void* logger, const char* msg) {
            static_cast<StreamLogger*>(logger)->stream.print(msg);
        }

        static void finalizeMessage(void* logger) {
            static_cast<StreamLogger*>(logger)->stream.println();
        }

    protected:
        Stream& stream;

        LogMessageStream sendMsgImpl(Level level) {
            stream.print('[');
            stream.print(millis());
            stream.print(' ');
            if (level <= Level::DEBUG) {
                stream.print("DEBUG");
            } else if (level <= Level::INFO) {
                stream.print("INFO");
            } else if (level <= Level::WARNING) {
                stream.print("WARNING");
            } else if (level <= Level::ERROR) {
                stream.print("ERROR");
            }
            stream.print("]  ");
            return LogMessageStream(this, &writeMessage, &finalizeMessage);
        }

    public:
        StreamLogger(Stream& stream, Level level) : Logger(level), stream(stream) {}
    };

#ifdef HAVE_HWSERIAL0

    class SerialLogger : public StreamLogger<HardwareSerial> {
    public:
        SerialLogger(uint32_t baudRate, Level level)
                : StreamLogger<HardwareSerial>(Serial, level) {
            stream.begin(baudRate);
        }
    };

#endif
}