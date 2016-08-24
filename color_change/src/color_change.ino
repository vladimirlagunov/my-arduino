#include <stdio.h>

const size_t channel_count = 3;
const int out_pins[channel_count] = {3, 5, 10};
const int tune_pins[channel_count] = {12, 13, 8};

unsigned char current_color[channel_count] = {0, 0, 0};
bool tune_pins_was_pressed[channel_count] = {0, 0, 0};
bool direction[channel_count] = {1, 1, 1};

void setup()
{
    for (size_t i = 0; i < channel_count; ++i) {
        pinMode(tune_pins[i], INPUT);
        tune_pins_was_pressed[i] = digitalRead(tune_pins[i]);
    }
    Serial.begin(9600);
}

void loop()
{
    bool any_button_released = false;
    for (size_t i = 0; i < channel_count; ++i) {
        bool tune_pin_pressed = digitalRead(tune_pins[i]);
        if (tune_pin_pressed) {
            if (direction[i]) {
                if (current_color[i] < 255) {
                    current_color[i]++;
                }
            } else {
                if (current_color[i] > 0) {
                    current_color[i]--;
                }
                
            }
        } else if (tune_pins_was_pressed[i]) {
            direction[i] = !direction[i];
            any_button_released = true;
        }
        tune_pins_was_pressed[i] = tune_pin_pressed;

        analogWrite(out_pins[i], current_color[i]);
    }
    if (any_button_released) {
        char buffer[256];
        sprintf(buffer, "Current color: Red: %d, Green: %d, Blue: %d", current_color[0], current_color[1], current_color[2]);
        Serial.println(buffer);
        Serial.flush();
    }
    delay(5);
}
