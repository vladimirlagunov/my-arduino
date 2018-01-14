#include <RCSwitch.h>
#include <time.h>

RCSwitch rcSwitch;

const int GPIO_PIN = GPIOR02;

void setup() {
    Serial.begin(9600);
    rcSwitch.enableReceive(GPIO_PIN - 2);  // strange rc-switch logic
}

time_t aliveTimeStart = 0;

void loop() {
    if (rcSwitch.available()) {
        Serial.print("Received ");
        Serial.print(rcSwitch.getReceivedValue());
        Serial.print(" / ");
        Serial.print(rcSwitch.getReceivedBitlength());
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.println(rcSwitch.getReceivedProtocol());
        rcSwitch.resetAvailable();
    }
}
