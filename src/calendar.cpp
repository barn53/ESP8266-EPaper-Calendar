#include "calendar.h"
#include "common.h"
#include <U8g2_for_Adafruit_GFX.h>

void printUpdate(DynamicJsonDocument& doc)
{
    display.setRotation(0);
    u8gfx.setFont(u8g2_font_helvR08_tf);
    uint16_t fontHeight(8);
    uint16_t x(0), y(display.height() - fontHeight - 6), w(150), h(fontHeight + 6);
    display.setPartialWindow(x, y, w, h);

    auto utc = doc["utc"].as<time_t>();
    auto utcToLocal = doc["utcToLocal"].as<time_t>();
    String updateTime(timeToString(utc + utcToLocal));

    u8gfx.setForegroundColor(GxEPD_WHITE);
    u8gfx.setBackgroundColor(GxEPD_BLACK);
    display.firstPage();
    do {
        display.fillScreen(GxEPD_BLACK);
        u8gfx.setCursor(x+7, y + fontHeight + 3);
        u8gfx.print("Stand: ");
        u8gfx.print(updateTime);
    } while (display.nextPage());
}

void printToDisplay(DynamicJsonDocument& doc)
{
    display.setFullWindow();
    display.setRotation(1);

    u8gfx.setFontMode(0); // use u8g2 transparent mode (this is default)
    u8gfx.setFontDirection(0); // left to right (this is default)
    u8gfx.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
    u8gfx.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color

    display.setTextWrap(false);

    auto status = doc["status"].as<const char*>();
    Serial.printf("Status: %s\n", status);

    display.firstPage();
    do {
        display.fillScreen(GxEPD_WHITE);
        int16_t y(0);
        bool once(true);
        String lastFromDate;

#define H1 u8g2_font_helvB24_tf
#define H2 u8g2_font_helvB14_tf
#define T1 u8g2_font_helvR12_tf
#define TB1 u8g2_font_helvB12_tf
#define T2 u8g2_font_helvR08_tf

        int16_t h1(24), h2(14), t1(12), t2(8);

        bool prevWasToday(false);
        for (auto e : doc["events"].as<JsonArray>()) {
            auto startsEarlier(e["startsEarlier"].as<bool>());
            auto today(e["today"].as<bool>());
            String fromDate(e["fromDate"].as<const char*>());
            if (startsEarlier && once) {
                once = false;
                u8gfx.setFont(H1);
                y += h1 + 6;
                u8gfx.setCursor(2, y);
                u8gfx.print("Aktuell");
                display.drawFastHLine(0, y + 1, display.width(), GxEPD_BLACK);
                y += 2;
            } else if (!startsEarlier) {
                if (lastFromDate != fromDate) {
                    lastFromDate = fromDate;
                    if (today) {
                        u8gfx.setFont(H1);
                        y += h1 + h1;
                        u8gfx.setCursor(2, y);
                        u8gfx.print("Heute");
                        prevWasToday = true;
                    } else if (prevWasToday) {
                        y += h1;
                        prevWasToday = false;
                    }
                    u8gfx.setFont(H2);
                    y += h2 + 12;
                    u8gfx.setCursor(2, y);
                    u8gfx.print(fromDate);
                    display.drawFastHLine(0, y + 1, display.width(), GxEPD_BLACK);
                    y += 2;
                }
            }

            auto isAllDay(e["isAllDay"].as<bool>());
            String title(e["title"].as<const char*>());
            String creator(e["creator"].as<const char*>());
            auto description(e["description"].as<const char*>());
            auto location(e["location"].as<const char*>());
            String toDate(e["toDate"].as<const char*>());

            if (creator.startsWith("eva.henneberger")) {
                title += " (Eva)";
            }

            y += t1 + 12;
            u8gfx.setCursor(10, y);

            if (isAllDay) {
                u8gfx.setFont(TB1);
                u8gfx.print(title);
                u8gfx.setFont(T1);
                if (startsEarlier
                    || fromDate != toDate) {
                    u8gfx.setFont(T2);
                    y += t2 + 4;
                    u8gfx.setCursor(25, y);
                    u8gfx.print("bis ");
                    u8gfx.print(toDate);
                }
            } else {
                String fromTime(e["fromTime"].as<const char*>());
                String toTime(e["toTime"].as<const char*>());
                if (startsEarlier) {
                    u8gfx.setFont(TB1);
                    u8gfx.print(title);
                    u8gfx.setFont(T2);
                    y += t2 + 4;
                    u8gfx.setCursor(25, y);
                    u8gfx.print("bis ");
                    u8gfx.print(toDate);
                    u8gfx.print(", ");
                    u8gfx.print(toTime);
                    u8gfx.print(" Uhr");
                } else {
                    u8gfx.setFont(T1);
                    u8gfx.print(fromTime);
                    if (fromDate != toDate) {
                        u8gfx.print(": ");
                        u8gfx.setFont(TB1);
                        u8gfx.print(title);
                        u8gfx.setFont(T2);
                        y += t2 + 4;
                        u8gfx.setCursor(25, y);
                        u8gfx.print("bis ");
                        u8gfx.print(toDate);
                        if (fromTime != toTime) {
                            u8gfx.print(", ");
                            u8gfx.print(toTime);
                            u8gfx.print(" Uhr");
                        }
                    } else if (fromTime != toTime) {
                        u8gfx.setFont(T1);
                        u8gfx.print("-");
                        u8gfx.print(toTime);
                        u8gfx.print(": ");
                        u8gfx.setFont(TB1);
                        u8gfx.print(title);
                    }
                }
            }

            if (strlen(location) > 0) {
                u8gfx.setFont(T2);
                y += t2 + 4;
                u8gfx.setCursor(25, y);
                u8gfx.print("(");
                u8gfx.print(location);
                u8gfx.print(")");
            }

            if (y > display.height()) {
                break;
            }
        }
    } while (display.nextPage());

    printUpdate(doc);
}
