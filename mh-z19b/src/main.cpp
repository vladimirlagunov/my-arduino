#include <ArduinoPlug.hpp>
#include <USBAPI.h>
#include <StreamLogger.hpp>
#include <SoftwareSerial.h>
#include <Timer.hpp>
#include <MhZ19b.hpp>


const uint8_t MHZ19B_SERIAL_RECEIVE_PIN = 8;
const uint8_t MHZ19B_SERIAL_TRANSMIT_PIN = 9;

class MHZ19BExample {
    logger::SerialLogger logger;
    mhz19b::Supervisor mhz19b;
    uint16_t oldCo2Ppm;

public:
    MHZ19BExample()
            : logger(9600, logger::Level::INFO),
              mhz19b(logger, MHZ19B_SERIAL_RECEIVE_PIN, MHZ19B_SERIAL_TRANSMIT_PIN),
              oldCo2Ppm{0}
    {}

    void loop() {
        mhz19b.loop();
        if (mhz19b.getCo2Ppm() != oldCo2Ppm) {
            oldCo2Ppm = mhz19b.getCo2Ppm();
            logger.info() << "CO2 PPM: " << oldCo2Ppm << logger::end;
        }
        delay(10);
    }
};

ArduinoPlug<MHZ19BExample> plug;

void setup() {
    plug.setup();
}

void loop() {
    plug.loop();
}