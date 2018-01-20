#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <RCSwitch.h>

#include <secret.hpp>

HardwareSerial& DBG_OUTPUT_PORT = Serial;

const int TRANSMITTER_PIN = 5;
const long msg = 1543874;

ESP8266WebServer server(80);
RCSwitch rcSwitch;

const char* mainPage = "\
<html>\
  <body style=\"background: black\">\
    <form method=post action=/toggle/>\
      <input type=submit value=toggle style=\"display: inline-block; text-align: center; width: 80%; margin-left: 10%; height: 80%; font-size: 64pt; background: forestgreen; foreground: white; border: 0px\" />\
    </form>\
  </body>\
</html>\
";

void setup(void) {
    DBG_OUTPUT_PORT.begin(9600);
    DBG_OUTPUT_PORT.print("\n");
    DBG_OUTPUT_PORT.setDebugOutput(true);

    //WIFI INIT
    DBG_OUTPUT_PORT.printf("Connecting to %s\n", SSID);
    WiFi.begin(SSID, WIFI_PASSWD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DBG_OUTPUT_PORT.print(".");
    }
    DBG_OUTPUT_PORT.println("");
    DBG_OUTPUT_PORT.print("Connected! IP address: ");
    DBG_OUTPUT_PORT.println(WiFi.localIP());

    DBG_OUTPUT_PORT.print("Open http://");
    DBG_OUTPUT_PORT.print(WiFi.localIP());
    DBG_OUTPUT_PORT.print("/\n");


    //SERVER INIT
    server.on("/", HTTP_GET, []() {
        DBG_OUTPUT_PORT.print("GET /\n");
        server.send(200, "text/html", mainPage);
    });

    server.on("/toggle/", HTTP_POST, []() {
        DBG_OUTPUT_PORT.print("POST /toggle/\n");
        rcSwitch.send(msg, 24);
        server.sendHeader("Location", "/");
        server.send(302, "text/plain", "");
    });

    server.onNotFound([](){
        server.send(404, "text/plain", "Not found");
    });

    rcSwitch.enableTransmit(TRANSMITTER_PIN);

    server.begin();
}

void loop(void) {
    server.handleClient();
}
