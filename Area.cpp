#include "Area.h"

Area::Area(Bar bar1, Bar bar2) : bar1(bar1), bar2(bar2) {}

void Area::transform(const Area from, const Area to, const uint32_t duration, const uint32_t elapsed)
{
	bar1.transform(from.bar1, to.bar1, duration, elapsed);
	bar2.transform(from.bar2, to.bar2, duration, elapsed);
}

void Area::draw(CRGB & leds)
{
	bar1.draw(leds);
	bar2.draw(leds);
}

