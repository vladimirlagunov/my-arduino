class ReleaseButton {
public:
    explicit ReleaseButton()
        : button_was_pressed(false)
    {
    }

    boolean shouldHandle(const unsigned int pin_state) {
        if (button_was_pressed == HIGH && pin_state == LOW) {
            button_was_pressed = pin_state;
            return true;
        } else {
            button_was_pressed = pin_state;
            return false;
        }
    }

protected:
    boolean button_was_pressed;
};


const uint8_t btn_pin = 2;
const uint8_t relay_pin = 3;
bool enabled = false;
ReleaseButton btn;

void setup() {
    pinMode(btn_pin, INPUT);
    pinMode(relay_pin, OUTPUT);
}

void loop() {
    if (btn.shouldHandle(digitalRead(btn_pin))) {
        enabled = !enabled;
        digitalWrite(relay_pin, enabled);
    }
    delay(1);
}