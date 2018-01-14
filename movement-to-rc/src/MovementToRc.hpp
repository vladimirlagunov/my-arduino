#include <RCSwitch.h>

#pragma once

template<class Logger>
class MovementToRc {
public:
    MovementToRc(Logger& logger, const uint8_t rcPin, const uint8_t movementGpioPin, const uint32_t movementRiseRcSignal, const uint32_t movementFallRcSignal)
            : logger(logger),
              movementGpioPin(movementGpioPin),
              movementRiseRcSignal(movementRiseRcSignal),
              movementFallRcSignal(movementFallRcSignal),
              oldState(false) {
        rcSwitch.enableTransmit(rcPin);
        pinMode(movementGpioPin, INPUT);
        logger.info("MovementToRc initialized");
    }

    void loop() {
        auto newState = (bool) digitalRead(movementGpioPin);
        if (newState != oldState) {
            if (newState) {
                rcSwitch.send(movementRiseRcSignal, 24);
                logger.info("Sent rise signal");
            } else {
                rcSwitch.send(movementFallRcSignal, 24);
                logger.info("Sent fall signal");
            }
            oldState = newState;
        }
    }

private:
    Logger& logger;
    RCSwitch rcSwitch;
    const uint8_t movementGpioPin;
    const uint32_t movementRiseRcSignal;
    const uint32_t movementFallRcSignal;
    bool oldState;
};
