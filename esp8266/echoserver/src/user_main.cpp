extern "C" {
#include <ets_sys.h>
#include <ip_addr.h>
#include <espconn.h>
#include <osapi.h>
#include <gpio.h>
#include <os_type.h>
#include <user_interface.h>
#include <driver/uart.h>

#include "fuck.h"
}

#include "user_config.h"


static char hostname[] = HOSTNAME;

static const char prefix[] = "You sent: ";
static_assert(sizeof(prefix) == 11, "should include zero symbol");
static const size_t buffer_len = 256;
static uint8 buffer[buffer_len];
static size_t buffer_offset;


static void ICACHE_FLASH_ATTR
echoserver_receive(void * arg, char * pdata, unsigned short len) {
    auto * sock = reinterpret_cast<espconn*>(arg);
    for (char * ptr = pdata; ptr < pdata + len; ++ptr) {
        if (buffer_offset == 0) {
            while (buffer_offset < sizeof(prefix) - 1) {
                buffer[buffer_offset] = prefix[buffer_offset];
                ++buffer_offset;
            }
        }
        buffer[buffer_offset++] = *ptr;
        if (*ptr == '\n' || buffer_offset >= buffer_len - 1) {
            espconn_send(sock, buffer, buffer_offset);
            buffer_offset = 0;
        }
    }
    if (buffer_offset > sizeof(prefix)) {
        espconn_send(sock, buffer, buffer_offset);
        buffer_offset = 0;
    }
}


static void ICACHE_FLASH_ATTR
echoserver_reconnect(void *arg, sint8 err) {

}


static void ICACHE_FLASH_ATTR
echoserver_disconnect(void *arg) {

}


static void ICACHE_FLASH_ATTR
echoserver_connect(void *arg) {
    auto* sock = reinterpret_cast<espconn*>(arg);
    buffer[0] = '\0';
    buffer_offset = 0;
    espconn_regist_recvcb(sock, echoserver_receive);
    espconn_regist_reconcb(sock, echoserver_reconnect);
    espconn_regist_disconcb(sock, echoserver_disconnect);
}


static void ICACHE_FLASH_ATTR
start_echo_server() {
    static struct espconn sock;
    static esp_tcp tcp;
    sock.type = ESPCONN_TCP;
    sock.state = ESPCONN_NONE;
    sock.proto.tcp = &tcp;
    tcp.local_port = 80;
    espconn_regist_connectcb(&sock, echoserver_connect);
    espconn_accept(&sock);
}


static inline void user_init_cpp() {
    struct station_config stationConf;
    wifi_set_opmode(STATION_MODE);
    strcpy((char*)stationConf.ssid, SSID);
    strcpy((char*)stationConf.password, SSID_PASSWORD);
    wifi_station_set_config(&stationConf);
    wifi_station_set_hostname(hostname);
    wifi_station_connect();

    uart_init(BIT_RATE_9600, BIT_RATE_9600);
    uint8 msg[] = "\nhello fucking world!\n";
    uart0_tx_buffer(msg, sizeof(msg));

    start_echo_server();
}


extern "C" {
void ICACHE_FLASH_ATTR user_init() {
    user_init_cpp();
}
}
