const int colors = 3;
const int rgb_out[colors] = {3, 5, 10};
const int rgb_values_max[colors] = {0xff, 0x2f, 0x8f};

void setup(void) {
    for (int i = 0; i < colors; ++i) {
        pinMode(rgb_out[i], OUTPUT);
    }
    randomSeed(analogRead(0));
    /* Serial.begin(9600); */
}

float rgb_parts[] = {0, 0, 0};
int current_color = 0;
int prev_color = colors - 1;
const float delta = 0.005;
const int loop_delay = 10;
const int pure_color_delay = 0;

void loop() {
    rgb_parts[current_color] += delta;
    if (rgb_parts[current_color] >= 1) {
        rgb_parts[current_color] = -1;
        prev_color = current_color;
        while (prev_color == current_color) {
            current_color = random(colors);
        }
        delay(pure_color_delay);
    } else {
        rgb_parts[prev_color] += delta;
        if (rgb_parts[prev_color] >= 0) {
            rgb_parts[prev_color] = 0;
        } else if (abs(rgb_parts[current_color] + rgb_parts[prev_color]) < delta) {
            delay(pure_color_delay);
        }
    }

    /* String s; */
    for (int i = 0; i < colors; ++i) {
        /* s += abs(int(rgb_parts[i] * rgb_values_max[i])); */
        /* s += ':'; */
        analogWrite(rgb_out[i], abs(int(rgb_parts[i] * rgb_values_max[i])));
    }
    /* Serial.println(s); */
    /* Serial.flush(); */

    delay(loop_delay);
}
