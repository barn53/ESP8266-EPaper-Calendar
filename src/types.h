#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <GxEPD2_BW.h>

#define MyGxEPD GxEPD2_750
using MyDisplay = GxEPD2_BW<MyGxEPD, MyGxEPD::HEIGHT / 16>;

// #define MyGxEPD GxEPD2_290
// using MyDisplay = GxEPD2_BW<MyGxEPD, MyGxEPD::HEIGHT>;
