#include <RCSwitch.h>
#include "ArduinoPlug.hpp"
#include "SerialLogger.hpp"
#include "MovementReaction.hpp"

const uint8_t MOVEMENT_GPIO_PIN = GPIOR05;
const uint32_t duration = 3 * 1000;
const uint32_t riseCycle = 1 * 1000 / 256;
const uint32_t fallCycle = 4 * 1000 / 256;
const uint8_t LED_PIN = GPIOR03;

class LedReaction {
    SerialLogger& serialLogger;
    uint8_t expectedValue;
    int8_t delta;
    uint8_t value;
    uint8_t valueCycle;
public:
    explicit LedReaction(SerialLogger& logger)
            : serialLogger(logger), expectedValue(0), delta(0), value(0), valueCycle(0) {
        pinMode(LED_PIN, OUTPUT);
    }

    void rise() {
        serialLogger.info("led on");
        delta = 1;
        expectedValue = 255;
        valueCycle = 0;
    }

    void fall() {
        serialLogger.info("led off");
        delta = -1;
        expectedValue = 0;
        valueCycle = 0;
    }

    void loop() {
        if (value != expectedValue) {
            if (valueCycle < (expectedValue > 0 ? riseCycle : fallCycle)) {
                delay(1);
                ++valueCycle;
            } else {
                value += delta;
                analogWrite(LED_PIN, value);
                valueCycle = 0;
            }
        }
    }
};

class Main {
    SerialLogger serialLogger;
    LedReaction ledReaction;
    MovementReaction<LedReaction> movementReaction;

public:
    Main() : serialLogger(9600),
             ledReaction(serialLogger),
             movementReaction(ledReaction, duration, MOVEMENT_GPIO_PIN) {
    }

    void loop() {
        movementReaction.loop();
        ledReaction.loop();
    }
};

ArduinoPlug<Main> plug;

void setup() {
    plug.setup();
}

void loop() {
    plug.loop();
}
