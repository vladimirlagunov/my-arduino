#pragma once

#include <WString.h>
#include <stdint.h>

namespace logger {
    enum class Level {
        DEBUG = 10,
        INFO = 20,
        WARNING = 30,
        ERROR = 40,
    };

    class _LogMessageEnd {
    };

    static _LogMessageEnd end = _LogMessageEnd();

    class LogMessageStream {
    protected:
        void* logger;

        void (* writeMessage)(void*, const char*);

        void (* finalizeMessage)(void*);

    public:
        LogMessageStream(void* logger, void (* writeMessageFn)(void*, const char*), void(* finalizeMessageFn)(void*))
                : logger(logger), writeMessage(writeMessageFn), finalizeMessage(finalizeMessageFn) {}

        LogMessageStream& operator<<(const char* msg) {
            writeMessage(logger, msg);
            return *this;
        }

        LogMessageStream& operator<<(const String& msg) { return *this << msg.c_str(); }

        LogMessageStream& operator<<(unsigned char msg) { return *this << String(msg); }

        LogMessageStream& operator<<(char msg) { return *this << String(msg); }

        LogMessageStream& operator<<(unsigned short msg) { return *this << String(msg); }

        LogMessageStream& operator<<(short msg) { return *this << String(msg); }

        LogMessageStream& operator<<(unsigned int msg) { return *this << String(msg); }

        LogMessageStream& operator<<(int msg) { return *this << String(msg); }

        LogMessageStream& operator<<(unsigned long msg) { return *this << String(msg); }

        LogMessageStream& operator<<(long msg) { return *this << String(msg); }

        LogMessageStream& operator<<(float msg) { return *this << String(msg); }

        LogMessageStream& operator<<(double msg) { return *this << String(msg); }

        LogMessageStream& operator<<(bool msg) { return *this << String(msg); }

        void operator<<(_LogMessageEnd end) { finalizeMessage(logger); }
    };

    class Logger {
    protected:
        static void dummyWriteMessage(void*, const char*) {}

        static void dummyFinalizeMessage(void*) {}

        const Level level;

        virtual LogMessageStream sendMsgImpl(Level level) = 0;

    public:
        explicit Logger(Level level) : level(level) {}

        LogMessageStream sendMsg(Level level) {
            if (this->level <= level) {
                return sendMsgImpl(level);
            } else {
                return LogMessageStream(this, &dummyWriteMessage, &dummyFinalizeMessage);
            }
        }

        LogMessageStream debug() { return sendMsg(Level::DEBUG); }

        LogMessageStream info() { return sendMsg(Level::INFO); }

        LogMessageStream warning() { return sendMsg(Level::WARNING); }

        LogMessageStream error() { return sendMsg(Level::ERROR); }
    };

    class DummyLogger : public Logger {
        static DummyLogger instance;

        virtual LogMessageStream sendMsgImpl(Level ignored1) {
            return LogMessageStream(this, &dummyWriteMessage, &dummyFinalizeMessage);
        }

        DummyLogger() : Logger(Level::INFO) {}

    public:
        static DummyLogger& singleton() {
            return instance;
        }
    };
}
