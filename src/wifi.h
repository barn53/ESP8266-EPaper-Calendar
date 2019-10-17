#pragma once
#include "types.h"

#ifdef ESP32
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#else
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#endif
#include <WiFiClient.h>

void wifiSleep();
void wifiWake();
void setupWiFi();
void getTime(DynamicJsonDocument& doc, time_t& startTime);
bool getCalendar(DynamicJsonDocument& doc);
