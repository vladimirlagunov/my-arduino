#include <RCSwitch.h>
#include "ArduinoPlug.hpp"
#include "SerialLogger.hpp"
#include "MovementToRc.hpp"

const int TRANSMITTER_GPIO_PIN = GPIOR02;
const uint8_t MOVEMENT_GPIO_PIN = GPIOR05;
const uint32_t ON_MOVEMENT_RISE = 171014;
const uint32_t ON_MOVEMENT_FALL = 171015;

class Main {
    SerialLogger serialLogger;
    MovementToRc<SerialLogger> movementToRc;

public:
    Main() : serialLogger(9600),
             movementToRc(serialLogger, TRANSMITTER_GPIO_PIN, MOVEMENT_GPIO_PIN, ON_MOVEMENT_RISE, ON_MOVEMENT_FALL) {
    }

    void loop() {
        movementToRc.loop();
        delay(1);
    }
};

ArduinoPlug<Main> plug;

void setup() {
    plug.setup();
}

void loop() {
    plug.loop();
}
