class SimpleStepper {
public:
    SimpleStepper(unsigned int pin1, unsigned int pin2, unsigned int pin3, unsigned int pin4)
        : current_step(0)
        , pins{pin1, pin2, pin3, pin4}
    {
    }

    void setPinMode() {
        pinMode(pins[0], OUTPUT);
        pinMode(pins[1], OUTPUT);
        pinMode(pins[2], OUTPUT);
        pinMode(pins[3], OUTPUT);
    }

    void stepForward() {
        current_step++;
        current_step &= 7;
        writePins();
    }

    void stepBackward() {
        current_step--;
        current_step &= 7;
        writePins();
    }

protected:
    unsigned int current_step;

    const unsigned int pins[4];
    const unsigned int all_steps[8][4] = {
        {0, 0, 0, 1},
        {0, 0, 1, 1},
        {0, 0, 1, 0},
        {0, 1, 1, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 1},
    };

    void writePins() {
        const unsigned int * const steps = all_steps[current_step];
        digitalWrite(pins[0], steps[0]);
        digitalWrite(pins[1], steps[1]);
        digitalWrite(pins[2], steps[2]);
        digitalWrite(pins[3], steps[3]);
    }
};

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


class AutoRepeatButton {
public:
    explicit AutoRepeatButton(unsigned long first_interval_ms = 800, unsigned long other_interval_ms = 125)
        : first_interval_ms(first_interval_ms)
        , other_interval_ms(other_interval_ms)
        , was_pressed(false)
        , next_event_micros(0)
    {
    }

    boolean shouldHandle(const unsigned int pin_state) {
        if (pin_state == HIGH) {
            unsigned long now = millis();
            if (next_event_micros <= now) {
                if (was_pressed == HIGH) {
                    next_event_micros = now + other_interval_ms;
                } else {
                    next_event_micros = now + first_interval_ms;
                }
                return true;
            }
        }
        was_pressed = pin_state;
        return false;
    }

protected:
    const unsigned long first_interval_ms;
    const unsigned long other_interval_ms;

    unsigned int was_pressed;
    unsigned long next_event_micros;
};

SimpleStepper stepper(8, 9, 10, 11);
const int inc_speed_btn_pin = 2;
const int dec_speed_btn_pin = 4;
const int direction_btn_pin = 5;
const unsigned int interval_step = 50;

unsigned int interval_mcs = 1300;
AutoRepeatButton inc_speed_btn;
AutoRepeatButton dec_speed_btn;
ReleaseButton direction_btn;
boolean direction = true;

void setup() {
    stepper.setPinMode();
    pinMode(inc_speed_btn_pin, INPUT);
    pinMode(dec_speed_btn_pin, INPUT);
    pinMode(direction_btn_pin, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (inc_speed_btn.shouldHandle(digitalRead(inc_speed_btn_pin))) {
        if (interval_mcs > interval_step) {
            interval_mcs -= interval_step;
        } else {
            interval_mcs = 0;
        }
        Serial.println("New interval: " + String(interval_mcs) + " microseconds");
        Serial.flush();
    }

    if (dec_speed_btn.shouldHandle(digitalRead(dec_speed_btn_pin))) {
        interval_mcs += interval_step;
        Serial.println("New interval: " + String(interval_mcs) + " microseconds");
        Serial.flush();
    }

    if (direction_btn.shouldHandle(digitalRead(direction_btn_pin))) {
        direction = !direction;
    }

    if (direction) {
        stepper.stepForward();
    } else {
        stepper.stepBackward();
    }
        
    delayMicroseconds(interval_mcs);
}