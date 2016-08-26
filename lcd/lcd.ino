#include <stdio.h>
#include <LiquidCrystal_I2C.h>

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

namespace RuChar {
    const uint8_t Ya = 0;
    const uint8_t E = 'E';
    const uint8_t B = 1;
    const uint8_t U = 2;
    const uint8_t G = 3;
    const uint8_t S = 'C';
    const uint8_t I1 = 4;
}

LiquidCrystal_I2C display(0x27, 16, 2);

void setup()
{
    display.begin();

    createChar(
        display,
        RuChar::Ya,
        " ####",
        "#   #",
        "#   #",
        " ####",
        "  # #",
        " #  #",
        "#   #",
        "     "
    );

    createChar(
        display,
        RuChar::B,
        "#####",
        "#    ",
        "#    ",
        "#### ",
        "#   #",
        "#   #",
        "#### ",
        "     "
    );

    createChar(
        display,
        RuChar::U,
        "#   #",
        "#   #",
        "#   #",
        " ####",
        "    #",
        "   # ",
        "###  ",
        "     "
    );

    createChar(
        display,
        RuChar::G,
        "#####",
        "#    ",
        "#    ",
        "#    ",
        "#    ",
        "#    ",
        "#    ",
        "     "
    );

    createChar(
        display,
        RuChar::I1,
        "  #  ",
        "#   #",
        "#   #",
        "#  ##",
        "# # #",
        "##  #",
        "#   #",
        "     "
    );

    display.clear();
    display.write('>');

    display.write(RuChar::Ya);
    display.write(' ');
    display.write(RuChar::E);
    display.write(RuChar::B);
    display.write(RuChar::U);
    display.write(' ');
    display.write(RuChar::G);
    display.write(RuChar::U);
    display.write(RuChar::S);
    display.write(RuChar::E);
    display.write(RuChar::I1);

    display.write('<');
    display.display();
}


void loop()
{
}
