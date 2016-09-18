#include <stdio.h>

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void * __brkval;


void setup() {
    Serial.begin(9600);
}


void loop() {
    int free_memory = 0;
    char buf[256];
    Serial.println("-----------");
    sprintf(buf, "&__bss_end = 0x%x", (int)&__bss_end);
    Serial.println(buf);
    sprintf(buf, "&__heap_start = 0x%x", (int)&__heap_start);
    Serial.println(buf);
    sprintf(buf, "__brkval = 0x%x", (int)&__brkval);
    Serial.println(buf);
    if (__brkval) {
        free_memory = (int)&free_memory - (int)__brkval;
    } else {
        free_memory = (int)&free_memory - (int)&__bss_end;
    }
    sprintf(buf, "free_memory = %d", free_memory);
    Serial.println(buf);
    Serial.flush();

    delay(5000);
}