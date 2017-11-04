#pragma once

#include "Bar.h"
#include <FastLED.h>

class Area
{
	Bar bar1;
	Bar bar2;

  public:
	Area(Bar bar1, Bar bar2);

	void transform(const Area from, const Area to, const uint32_t duration, const uint32_t elapsed);

	void draw(struct CRGB * leds, const struct CRGB & color);
};

