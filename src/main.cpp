
// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V
// NOTE: connect 4.7k pull-down from GPIO15 to GND if your board or shield has level converters
// NOTE for ESP8266: using SS (GPIO15) for CS may cause boot mode problems, use different pin in case, or 4k7 pull-down

// base class GxEPD2_GFX can be used to pass references or pointers to the display instance as parameter, uses ~1.2k more code
// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>

#include "calendar.h"
#include "common.h"
#include "wifi.h"

time_t startTime;

MyDisplay display(MyGxEPD(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));
U8G2_FOR_ADAFRUIT_GFX u8gfx;

// https://arduinojson.org/v6/assistant/
DynamicJsonDocument calendarDoc(JSON_ARRAY_SIZE(15) + JSON_OBJECT_SIZE(4) + 15 * JSON_OBJECT_SIZE(11) + 3000);
DynamicJsonDocument timeDoc(JSON_OBJECT_SIZE(15) + 350);

void printConnecting(int state)
{
    display.setRotation(1);
    display.setFont(&FreeSansBold18pt7b);
    display.setTextColor(GxEPD_BLACK);
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Connecting...", 0, 0, &tbx, &tby, &tbw, &tbh);

    if (state == 0) {
        uint16_t x((display.width() - tbw) / 2), y((display.height() - tbh) / 2), w(tbw), h(tbh);
        display.setPartialWindow(x, y, w, h);
        display.firstPage();
        do {
            display.fillScreen(GxEPD_WHITE);
            display.setCursor(x, y - tby);
            display.print("Connecting...");
        } while (display.nextPage());
    } else if (state == 1) {
        uint16_t x(5), y((display.height() / 2) + (tbh * 2)), w(display.width() - x), h(tbh);
        display.setPartialWindow(x, y, w, h);
        display.firstPage();
        do {
            display.fillScreen(GxEPD_WHITE);
            display.setCursor(x, y - tby);
            display.print("IP: ");
            display.print(WiFi.localIP().toString());
        } while (display.nextPage());
    }
}

void setup()
{
    wifiSleep();

    Serial.begin(115200);
    display.init(115200);
    u8gfx.begin(display);

    printConnecting(0);

    setupWiFi();
    printConnecting(1);
    // getTime(timeDoc, startTime);
    getCalendar(calendarDoc);

    wifiSleep();

    printToDisplay(calendarDoc);

    display.hibernate();
    ESP.deepSleep(20e6, WAKE_RF_DISABLED);
}

void loop()
{
}
