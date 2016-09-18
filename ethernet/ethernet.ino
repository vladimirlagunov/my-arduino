#include <EtherCard.h>
#include <stdio.h>
#include "rgb_url.hpp"


// PIN Connections (Using Arduino UNO):
//-   VCC -    3.3V
//+   VCC -    5V
//    GND -    GND
//    SCK - Pin 13
//    SO  - Pin 12
//    SI  - Pin 11
//    CS  - Pin  8
//+   RST - RES


const uint8_t CS_PIN = 8;
const uint8_t macaddr[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};
const uint16_t buffer_size = 700;
uint8_t Ethernet::buffer[buffer_size];

const char hostname[] PROGMEM = "my-arduino";

const uint8_t RED_LED = 3;
const uint8_t GREEN_LED = 4;
const uint8_t BLUE_LED = 5;


void setup() {
    Serial.begin(57600);

    uint8_t firmware_version = ether.begin(buffer_size, macaddr, CS_PIN);
    Serial.print(F("ENC28J60 Firmware Version: "));
    Serial.println(String(firmware_version));

    while (!ether.dhcpSetup(hostname)) {
        Serial.println(F("DHCP Fail"));
    }
    Serial.println(F("DHCP OK"));

    Serial.print(F("IP: "));
    for (int i = 0; i < IP_LEN; ++i) {
        if (i) {
            Serial.write('.');
        }
        Serial.print(String(ether.myip[i]));
    }
    Serial.print(F("\r\n"));
    Serial.flush();

    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
}


const char MAIN_PAGE[] PROGMEM =
    "HTTP/1.0 200 OK\r\n"
    "\r\n"
    "<!DOCTYPE html>"
    "<html><body>"
    "<script src='https://yastatic.net/jquery/3.1.0/jquery.min.js'></script>"
    "<script>"
    "function setIt(e,c){"
    "var r={};r[c]=e.checked?1:0;"
    "$.get('/set',r).done(parseIt);"
    "};"
    "function parseIt(s) {"
    "var r=s.match(/r=([01])&g=([01])&b=([01])/);"
    "var c='checked';"
    "$('#r').prop(c,r[1]=='1'?c:'');"
    "$('#g').prop(c,r[2]=='1'?c:'');"
    "$('#b').prop(c,r[3]=='1'?c:'');"
    "}"
    "$.get('/set').done(parseIt);"
    "</script>"
    "<p>R<input type=\"checkbox\" id=\"r\" onchange=\"setIt(this,'r')\"/></p>"
    "<p>G<input type=\"checkbox\" id=\"g\" onchange=\"setIt(this,'g')\"/></p>"
    "<p>B<input type=\"checkbox\" id=\"b\" onchange=\"setIt(this,'b')\"/></p>"
    "</body></html>"
    ;

const char NOT_FOUND_PAGE[] PROGMEM =
    "HTTP/1.0 404 Not Found\r\n"
    "\r\n"
    "<html><body><h1>404 Not Found</h1></body></html>"
    ;


const char METHOD_NOT_ALLOWED_PAGE[] PROGMEM =
    "HTTP/1.0 405 Method Not Allowed\r\n"
    "\r\n"
    "<html><body><h1>405 Method Not Allowed</h1></body></html>"
    ;

const char SET_RESULT_START[] PROGMEM =
    "HTTP/1.0 200 OK\r\n"
    "\r\n"
    ;


RgbUrl rgb;


void loop() {
    uint16_t pos = ether.packetLoop(ether.packetReceive());
    if (pos) {
        char* data = (char*)ether.buffer + pos;
        if (strncmp(data, "GET /", 5) == 0) {
            data += 5;
            if (data[0] == ' ') {
                memcpy_P(ether.tcpOffset(), MAIN_PAGE, sizeof MAIN_PAGE);
                ether.httpServerReply(sizeof MAIN_PAGE - 1);
            } else if (strncmp(data, "set", 3) == 0) {
                data += 3;
                if (*data == '?') {
                    ++data;
                    rgb.from_query(data, sizeof ether.buffer - pos - 9);
                }
                memcpy_P(ether.tcpOffset(), SET_RESULT_START, sizeof SET_RESULT_START);
                char buf[16];
                rgb.dump(buf);
                memcpy(ether.tcpOffset() + sizeof SET_RESULT_START, buf, sizeof buf);
                ether.httpServerReply(sizeof SET_RESULT_START + strlen(buf));
            } else {
                memcpy_P(ether.tcpOffset(), NOT_FOUND_PAGE, sizeof NOT_FOUND_PAGE);
                ether.httpServerReply(sizeof NOT_FOUND_PAGE - 1);
            }
        } else {
            memcpy_P(ether.tcpOffset(), METHOD_NOT_ALLOWED_PAGE, sizeof METHOD_NOT_ALLOWED_PAGE);
            ether.httpServerReply(sizeof METHOD_NOT_ALLOWED_PAGE - 1);
        }
    }

    digitalWrite(RED_LED, rgb.red);
    digitalWrite(GREEN_LED, rgb.green);
    digitalWrite(BLUE_LED, rgb.blue);
}
