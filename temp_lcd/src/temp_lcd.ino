#include <DHT.h>
#include <LiquidCrystal_I2C.h>

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


constexpr uint8_t make_row(const char* line) {
    return ( (((line[0] != ' ') & 1) << 4)
           | (((line[1] != ' ') & 1) << 3)
           | (((line[2] != ' ') & 1) << 2)
           | (((line[3] != ' ') & 1) << 1)
           | ((line[4] != ' ') & 1)
    );
}

inline void createChar(
    LiquidCrystal_I2C display,
    const uint8_t letter,
    const char * const l0,
    const char * const l1,
    const char * const l2,
    const char * const l3,
    const char * const l4,
    const char * const l5,
    const char * const l6,
    const char * const l7
)
{
    uint8_t letter_ = letter;
    uint8_t buf_[8] = {
        make_row(l0),
        make_row(l1),
        make_row(l2),
        make_row(l3),
        make_row(l4),
        make_row(l5),
        make_row(l6),
        make_row(l7),
    };
    display.createChar(letter_, buf_);
}

namespace MyChars {
    const uint8_t Temperature = 0;
    const uint8_t Degree = 1;
    const uint8_t Drop = 2;
}


/*
  DHT11 QC:
  S (left) = data
  + (middle) = 5V
  - (right) = GND
 */

const uint8_t dht_pin = 2;
DHT dht(dht_pin, DHT11);
const uint8_t btn_pin = 8;
ReleaseButton backlight_btn;
LiquidCrystal_I2C display(0x27, 16, 2);


void setup()
{
    pinMode(btn_pin, INPUT);

    dht.begin();

    display.begin();
    createChar(
        display,
        MyChars::Temperature,
        "  #  ",
        " # # ",
        " # # ",
        " ### ",
        " ### ",
        "#####",
        "#####",
        " ### "
    );

    createChar(
        display,
        MyChars::Degree,
        " ### ",
        "#   #",
        "#   #",
        " ### ",
        "     ",
        "     ",
        "     ",
        "     "
    );

    createChar(
        display,
        MyChars::Drop,
        "  #  ",
        "   # ",
        "  ## ",
        " ####",
        " ####",
        "#####",
        " ### ",
        "     "
    );

    display.backlight();
    display.clear();
    display.printstr("...");
    display.display();

    delay(2000);
}

int temp = 0;
int hum = 0;

bool backlight_enabled = true;
unsigned long last_update = 0;
const unsigned long check_delay = 2000;

void loop()
{
    if (backlight_btn.shouldHandle(digitalRead(btn_pin))) {
        backlight_enabled = !backlight_enabled;
        if (backlight_enabled) {
            display.backlight();
        } else {
            display.noBacklight();
        }
    }
    unsigned long next_check = last_update + check_delay;
    unsigned long now = millis();
    if (next_check <= check_delay) {
        // possibly integer overflow
        next_check += check_delay + check_delay;
        next_check += check_delay + check_delay;
    }
    if (next_check < now) {
        temp = dht.readTemperature();
        hum = dht.readHumidity();
        display.clear();

        display.write(MyChars::Temperature);
        display.write(' ');
        display.write('0' + temp / 10);
        display.write('0' + temp % 10);
        display.write(MyChars::Degree);
        display.write('C');

        display.setCursor(0, 1);
        display.write(MyChars::Drop);
        display.write(' ');
        display.write('0' + hum / 10);
        display.write('0' + hum % 10);
        display.write('%');

        display.display();
        last_update = millis();
    }
    delay(50);
}