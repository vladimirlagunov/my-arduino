#include <RCSwitch.h>

#pragma once

template<class Reaction>
class MovementReaction {
public:
    MovementReaction(Reaction& reaction, uint32_t durationMillis, const uint8_t movementGpioPin)
            : reaction(reaction),
              duration(durationMillis),
              movementGpioPin(movementGpioPin),
              lastRiseMillis(0),
              oldState(false) {
        pinMode(movementGpioPin, INPUT);
    }

    void loop() {
        auto newState = (bool) digitalRead(movementGpioPin);
        if (newState) {
            lastRiseMillis = millis();
            bool rose = oldState;
            oldState = true;
            if (!rose) {
                reaction.rise();
            }
        } else if (oldState && lastRiseMillis + duration <= millis()) {
            oldState = false;
            reaction.fall();
        }
    }

private:
    Reaction& reaction;
    const uint32_t duration;
    const uint8_t movementGpioPin;
    unsigned long lastRiseMillis;
    bool oldState;
};
