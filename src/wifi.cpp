#include "wifi.h"
#include "secrets.h"
#include <ArduinoJson.h>

void wifiSleep()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);
}
void wifiWake()
{
    WiFi.forceSleepWake();
    delay(1);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
}

void setupWiFi()
{
    wifiWake();

    WiFi.begin(ssid, password);
    WiFi.hostname(hostname);

    Serial.print("Connecting ");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.printf("\nConnected\n IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf(" Hostname: %s\n", WiFi.hostname().c_str());
}

bool getJsonFromUrl(const char* host, const char* uri, uint16_t port, DynamicJsonDocument& doc)
{
    bool ret(false);
    WiFiClient client;
    HTTPClient http;

    if (http.begin(client, host, port, uri, false)) {
        Serial.print("[HTTP] begin...\n");
        int httpCode = http.GET();
        if (httpCode > 0) {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if (httpCode == HTTP_CODE_OK) {
                deserializeJson(doc, client);
                ret = true;
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    }

    if (!ret) {
        Serial.printf("[HTTP} Unable to connect\n");
    }

    return ret;
}

void getTime(DynamicJsonDocument& doc, time_t& startTime)
{
    if (getJsonFromUrl("worldtimeapi.org", "/api/ip", 80, doc)) {
        auto unixtime = doc["unixtime"].as<time_t>();
        auto raw_offset = doc["raw_offset"].as<time_t>();
        auto dst_offset = doc["dst_offset"].as<time_t>();
        startTime = unixtime + raw_offset + dst_offset - (millis() / 1000);
    }
}

bool getCalendar(DynamicJsonDocument& doc)
{
    return getJsonFromUrl("rwmmvdid1hvmqy6p.myfritz.net", "/calendar", 4418, doc);
}
