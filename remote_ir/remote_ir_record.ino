const uint8_t ir_pin = 2;

void setup() {
    Serial.begin(9600);
}


unsigned int previous = 0;
unsigned long last_micros = 0;
unsigned long time = 0;
uint8_t new_ = 0;

const unsigned long dump_duration_micros = 200000;
const size_t buffer_capacity = 256;
size_t buffer_size = 0;
unsigned long micros_buffer[buffer_capacity];
uint8_t values[buffer_capacity];


void loop() {
    new_ = ~digitalRead(ir_pin) & 1;
    time = micros();

    if (new_ != previous) {
        micros_buffer[buffer_size] = last_micros;
        values[buffer_size] = previous;
        buffer_size++;
        micros_buffer[buffer_size] = time;
        values[buffer_size] = new_;
        buffer_size++;
        previous = new_;
    }

    if (
        buffer_size > 0
        &&
        (
            micros_buffer[buffer_size - 1] + dump_duration_micros < time
            ||
            buffer_size >= buffer_capacity - 1
        )
    ) {
        for (size_t i = 0; i < buffer_size; ++i) {
            Serial.println(String(micros_buffer[i]) + ' ' + String(values[i]));
        }
        Serial.flush();
        buffer_size = 0;
    }

    last_micros = time;
}
