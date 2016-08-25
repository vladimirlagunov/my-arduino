const unsigned int pin = 13;

void setup() {
    pinMode(pin, OUTPUT);
}

bool state = false;

void loop() {
    digitalWrite(pin, state);
    state = !state;
    delay(1000);
}