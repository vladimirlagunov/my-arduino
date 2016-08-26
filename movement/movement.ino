#include <stdio.h>

// hc-sr501 calibration time - 2 min on each start
// http://iarduino.ru/shop/Sensory-Datchiki/infrakrasnyy-datchik-dvizheniya.html
const uint8_t movement_sensor_pin = 2;


char debug_buf[256];


class FadeLED {
public:
    FadeLED(const uint8_t pin, const unsigned long step_duration)
        : pin_(pin)
        , step_duration_(step_duration)
        , max_duration_(255 * step_duration)
        , run_(false)
        , start_millis_(0)
    {
    }

    void start() {
        run_ = true;
        start_millis_ = millis();
        /* Serial.println(String(pin_) + ": start " + String(start_millis_)); */
    }

    void stop() {
        analogWrite(pin_, 0);
        run_ = false;
        /* Serial.println(String(pin_) + ": stop"); */
    }

    void loop() {
        if (run_) {
            const unsigned long delta = millis() - start_millis_;
            if (delta >= max_duration_) {
                stop();
            } else {
                analogWrite(pin_, 255 - delta / step_duration_);
                /* Serial.println(String(pin_) + ": write " + String(255 - delta / step_duration_)); */
            }
        }
        delay(1);
    }

private:
    const uint8_t pin_;
    const unsigned long step_duration_;
    const unsigned long max_duration_;
    bool run_;
    unsigned long start_millis_;
};


class FunnyAction {
public:
    FunnyAction(FadeLED * const leds, const size_t count)
        : leds(leds)
        , count(count)
    {
    }

    void setup() {
    }

    void loop(const bool enabled) {
        if (enabled) {
            for (size_t i = 0; i < count; ++i) {
                leds[i].start();
            }
        }
        for (size_t i = 0; i < count; ++i) {
            leds[i].loop();
        }
    }

private:
    FadeLED * const leds;
    const size_t count;
};

const size_t led_count = 3;
FadeLED leds[led_count] = {FadeLED(4, 30), FadeLED(5, 30), FadeLED(6, 35)};
FunnyAction action(leds, led_count);

void setup() {
    /* Serial.begin(9600); */
    action.setup();
    pinMode(movement_sensor_pin, INPUT);
}

void loop() {
    action.loop(digitalRead(movement_sensor_pin));
}