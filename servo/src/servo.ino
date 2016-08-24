#include <Servo.h>

Servo servo;

void setup()
{
    servo.attach(6);
}

const int angles[] = {0, 10, 20, 30, 40, 50, 70, 90, 110, 140, 180};
const int delay_ms = 500;

void loop()
{
    for (size_t i = 0; i < sizeof(angles) / sizeof(int); ++i) {
        servo.write(angles[i]);
        delay(delay_ms);
    }
}
