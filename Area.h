// Area.h

#ifndef _AREA_h
#define _AREA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

class Area
{
	Bar bar1;
	Bar bar2;

  public:
	Area(Bar bar1, Bar bar2);

	void transform(const Area from, const Area to, const uint32_t duration, const uint32_t elapsed);

	void draw(CRGB & leds);
};

#endif

