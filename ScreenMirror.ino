#include "espsm_conf.h"
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

WebSocketsServer webSocket(WEBSOCKET_PORT);
Arduino_DataBus *bus = new Arduino_ESP32SPI(BUS_CS, BUS_DC, BUS_RST, BUS_WR, BUS_RD);
Arduino_GFX *gfx = new Arduino_ST7789(bus, -1, LCD_ROTATION, true, LCD_H_RES, LCD_V_RES);

uint8_t* raw_buffer;
size_t received_bytes = 0;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if(type == WStype_BIN){
    if(received_bytes + length <= RAW_BUFFER_SIZE){
      memcpy(raw_buffer + received_bytes, payload, length);
      received_bytes += length;
    }
    if(received_bytes >= RAW_BUFFER_SIZE){
      gfx->draw16bitRGBBitmap(0, 0, (uint16_t*)raw_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
      received_bytes = 0;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LCD_BL_PIN, OUTPUT);
  analogWrite(LCD_BL_PIN, LCD_BL_BRIGHTNESS);

  gfx->begin();
  gfx->fillScreen(gfx->color565(0,0,0));
  gfx->setRotation(LCD_ROTATION);
  gfx->setTextColor(gfx->color565(0,255,0));
  gfx->setTextSize(2);
  gfx->setCursor(10, 10);
  gfx->print("Connecting to the Wi-Fi");

  raw_buffer = (uint8_t*) heap_caps_malloc(RAW_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
  if(!raw_buffer){
    gfx->fillScreen(gfx->color565(255,0,0));
    gfx->setCursor(10, 10);
    gfx->print("Memory error");
    while(1);
  }

  int dots = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    gfx->print(".");
    dots++;
    if(dots>50){
      gfx->fillScreen(gfx->color565(255,0,0));
      gfx->setCursor(10,10);
      gfx->print("Timeout error");
      while(1);
    }
  }

  gfx->fillScreen(gfx->color565(0,0,0));
  gfx->setCursor(10, 10);
  gfx->print("Connected!");
  gfx->setCursor(10, 30);
  gfx->print(WiFi.localIP());
  delay(1000);
  gfx->setCursor(10, 50);
  gfx->print("Waiting For Client...");
  gfx->setCursor(10, 70);
  gfx->print("Download Client Here: (nolink)");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}
