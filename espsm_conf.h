#ifndef CONFIG_H
#define CONFIG_H

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
const int WEBSOCKET_PORT = 81;

#define LCD_ROTATION 1
#define LCD_H_RES 240
#define LCD_V_RES 320
#define LCD_BL_PIN 1
#define LCD_BL_BRIGHTNESS 200

#define BUS_CS 42
#define BUS_DC 45
#define BUS_RST 39
#define BUS_WR 38
#define BUS_RD 40

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define RAW_BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * 2)

#endif
