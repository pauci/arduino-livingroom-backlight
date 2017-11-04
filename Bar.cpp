#include "Bar.h"

Bar::Bar(uint16_t startPos16, uint16_t endPos16) : startPos16(startPos16), endPos16(endPos16) {}

void Bar::transform(const Bar & from, const Bar & to, const uint32_t duration, const uint32_t elapsed)
{
	startPos16 = from.startPos16 + (to.startPos16 - from.startPos16) * elapsed / duration;
	endPos16 = from.endPos16 + (to.endPos16 - from.endPos16) * elapsed / duration;
}

void Bar::draw(CRGB & leds)
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

	for (int i = firstLed; i <= lastLed; ++i) {
		CRGB c = color;

		if (i == firstLed) {
			c.nscale8(firstBrightness);
		} else if (i == lastLed) {
			c.nscale8(lastBrightness);
		}

		leds[i] += c;
	}
}

