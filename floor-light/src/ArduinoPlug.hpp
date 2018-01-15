#pragma once
template <class Program>
class ArduinoPlug {
    Program* instance = nullptr;

public:
    void setup() {
        instance = new Program();
    }

    void loop() {
        instance->loop();
    }
};
