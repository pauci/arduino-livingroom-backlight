// Bar.h

#ifndef _BAR_h
#define _BAR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

class Bar
{
    uint16_t startPos16;
    uint16_t endPos16;

  public:
    Bar(uint16_t startPos16, uint16_t endPos16);

    void transform(const Bar & from, const Bar & to, const uint32_t duration, const uint32_t elapsed);

    void draw(CRGB & leds);
};

#endif

