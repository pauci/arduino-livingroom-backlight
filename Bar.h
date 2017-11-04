#pragma once

#include <FastLED.h>

class Bar
{
	long startPos16;
	long endPos16;

	long map(const long x, const long in_max, const long out_min, const long out_max);

  public:
    Bar(long startPos16, long endPos16);

    void transform(const Bar & from, const Bar & to, const long duration, const long elapsed);

    void draw(struct CRGB * leds, const struct CRGB & color);
};

