#pragma once
#include "types.h"

class U8G2_FOR_ADAFRUIT_GFX;

extern MyDisplay display;
extern U8G2_FOR_ADAFRUIT_GFX u8gfx;

void printToDisplay(DynamicJsonDocument& doc);
