#include <RCSwitch.h>
#include "ArduinoPlug.hpp"
#include "StreamLogger.hpp"
#include "MovementReaction.hpp"

const uint8_t MOVEMENT_GPIO_PIN = GPIOR05;
const uint32_t duration = 3 * 1000;
const uint32_t riseCycle = 1 * 1000 / 256;
const uint32_t fallCycle = 4 * 1000 / 256;
const uint8_t LED_PIN = GPIOR03;

class LedReaction {
    logger::Logger& logger;
    uint8_t expectedValue;
    int8_t delta;
    uint8_t value;
    uint8_t valueCycle;
public:
    explicit LedReaction(logger::Logger& logger)
            : logger(logger), expectedValue(0), delta(0), value(0), valueCycle(0) {
        pinMode(LED_PIN, OUTPUT);
    }

    void rise() {
        logger.info() << "led on" << logger::end;
        delta = 1;
        expectedValue = 255;
        valueCycle = 0;
    }

    void fall() {
        logger.info() << "led off" << logger::end;
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
    logger::SerialLogger logger;
    LedReaction ledReaction;
    MovementReaction<LedReaction> movementReaction;

public:
    Main() : logger(9600, logger::Level::INFO),
             ledReaction(logger),
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
