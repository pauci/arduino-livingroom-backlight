#include "Bar.h"

Bar::Bar(long startPos16, long endPos16) : startPos16(startPos16), endPos16(endPos16) {}

void Bar::transform(const Bar & from, const Bar & to, const long duration, const long elapsed)
{
	startPos16 = map(elapsed, duration, from.startPos16, to.startPos16);
	endPos16 = map(elapsed, duration, from.endPos16, to.endPos16);
}

long Bar::map(const long x, const long in_max, const long out_min, const long out_max)
{
	return x * (out_max - out_min) / in_max + out_min;
}

void Bar::draw(struct CRGB * leds, const struct CRGB & color)
{
	if (startPos16 >= endPos16) {
		return;
	}

	uint8_t firstFrac = startPos16 & 0x0F;
	uint8_t firstBrightness = 255 - (firstFrac * 16);

	uint8_t lastFrac = endPos16 & 0x0F;
	uint8_t lastBrightness = lastFrac * 16;

	int firstLed = startPos16 / 16;
	int lastLed = endPos16 / 16;

	if (firstLed == lastLed) {
		firstBrightness = (lastFrac - firstFrac) * 16;
	}

	CRGB c;
	for (int i = firstLed; i <= lastLed; ++i) {
		c = color;

		if (i == firstLed) {
			c = color;
			c.nscale8(firstBrightness);
			leds[i] += c;
		} else if (i == lastLed) {
			c = color;
			c.nscale8(lastBrightness);
			leds[i] += c;
		} else {
			leds[i] = color;
		}

		leds[i] = c;
	}
}

