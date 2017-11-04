#include "Area.h"
#include "Bar.h"
#define MY_GATEWAY_SERIAL

#include <MySensors.h>

#define SN "Livingroom Backlight"
#define SV "1.0"

#define CHILD_ID_LIGHT 1

#include <FastLED.h>

#define NUM_LEDS 490
#define NUM_LEDS_PER_STRIP 64
#define NUM_STRIPS 8

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#define LIGHT_OFF 0
#define LIGHT_ON 1

int16_t state = LIGHT_OFF;
int16_t brightness = 50;
CRGB color = CRGB::LightYellow;

#define MODE_ALL       0
#define MODE_CEILING   1
#define MODE_WALL      2
#define MODE_TV        3
#define MODE_TV_SIDES  4
#define MODE_TV_BOTTOM 5

uint8_t lastMode = MODE_ALL;
uint8_t mode = MODE_ALL;

#define BOUND_0 16 * 0
#define BOUND_1 16 * 94
#define BOUND_2 16 * 142
#define BOUND_3 16 * 179
#define BOUND_4 16 * 196
#define BOUND_5 3368 // 16 * 210.5
#define BOUND_6 16 * 225
#define BOUND_7 16 * 242
#define BOUND_8 16 * 279
#define BOUND_9 16 * 360
#define BOUND_10 16 * NUM_LEDS

const Area modes[] = {
	{ Bar(BOUND_0, BOUND_5), Bar(BOUND_5, BOUND_10) }, // MODE_ALL
	{ Bar(BOUND_0, BOUND_2), Bar(BOUND_8, BOUND_10) }, // MODE_CEILING
	{ Bar(BOUND_2, BOUND_5), Bar(BOUND_5, BOUND_8) },  // MODE_WALL
	{ Bar(BOUND_3, BOUND_5), Bar(BOUND_5, BOUND_7) },  // MODE_TV
	{ Bar(BOUND_3, BOUND_4), Bar(BOUND_6, BOUND_7) },  // MODE_TV_SIDES
	{ Bar(BOUND_4, BOUND_5), Bar(BOUND_5, BOUND_6) }   // MODE_TV_BOTTON
};

Area active = modes[MODE_ALL];
Area lastActive = modes[MODE_ALL];

#define MODE_TRANSITION_DURATION 3000
uint32_t modeTransitionStart;
bool transition = false;


MyMessage status_msg(CHILD_ID_LIGHT, V_STATUS);
MyMessage brightness_msg(CHILD_ID_LIGHT, V_PERCENTAGE);
MyMessage rgb_msg(CHILD_ID_LIGHT, V_RGB);
MyMessage mode_msg(CHILD_ID_LIGHT, V_VAR1);

char buf[6];

bool initialValueSent = false;

void presentation()
{
	sendSketchInfo(SN, SV);

	present(CHILD_ID_LIGHT, S_RGB_LIGHT);
}	

void setup()
{
	LEDS.addLeds<WS2811_PORTD, NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
	LEDS.setBrightness(brightness * 255 / 100);
	LEDS.clear(true);
}

void loop()
{
	if (!initialValueSent) {
		send_all();
		initialValueSent = true;
	}

	LEDS.delay(10);
	updateTansition();
	LEDS.show();
}

void receive(const MyMessage &message)
{
	if (message.sensor == CHILD_ID_LIGHT) {
		if (message.type == V_STATUS) {
			setState(message.getBool());
			send_status();
		}

		if (message.type == V_PERCENTAGE) {
			setBrightness(message.getInt());
			send_brightness();
		}

		if (message.type == V_RGB) {
			String hexstring = message.getString();
			setColor(strtol(&hexstring[0], NULL, 16));
			send_color();
		}

		if (message.type == V_VAR1) {
			setMode(message.getInt());
			send_mode();
		}
	}
}

void setState(uint8_t newState)
{
	if (newState == state) {
		return;
	}

	state = newState;
	draw();
}

void setBrightness(uint8_t newBrightness)
{
	if (newBrightness == brightness) {
		return;
	}

	LEDS.setBrightness(newBrightness * 255 / 100);
	brightness = newBrightness;
}

void setColor(CRGB newColor)
{
	if (newColor == color) {
		return;
	}

	color = newColor;
	draw();
}

void setMode(uint8_t newMode)
{
	if (newMode == mode) {
		return;
	}

	lastMode = mode;
	lastActive = active;

	mode = newMode;
	modeTransitionStart = millis();
	transition = true;
}

void updateTansition()
{
	if (!transition || state == LIGHT_OFF) {
		return;
	}
		
	uint32_t elapsedTime = millis() - modeTransitionStart;

	if (elapsedTime >= MODE_TRANSITION_DURATION) {
		transition = false;
		elapsedTime = MODE_TRANSITION_DURATION;
	}

	active.transform(lastActive, modes[mode], MODE_TRANSITION_DURATION, elapsedTime);

	draw();
}

void draw()
{
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = CRGB::Black;
	}

	if (state == LIGHT_ON) {
		active.draw(leds, color);
	}
}


void send_all()
{
	send_status();
	send_brightness();
	send_color();
	send_mode();
}

void send_status()
{
	send(status_msg.set(state));
}

void send_brightness()
{
	send(brightness_msg.set(brightness));
}

void send_color()
{
	sprintf(buf, "%02X%02X%02X", color.r, color.g, color.b);
	send(rgb_msg.set(buf));
}

void send_mode()
{
	send(mode_msg.set(mode));
}

