#define MY_GATEWAY_SERIAL

#include <MySensors.h>

#define SN "Livingroom backlight"
#define SV "1.0"

#define CHILD_ID_BRIGHTNESS 1
#define CHILD_ID_SEG_1 11 // Child ID of a first segment. The following segments have an incremental ID
#define CHILD_ID_IR 31


#include <FastLED.h>

#define NUM_LEDS 490
#define NUM_LEDS_PER_STRIP 64
#define NUM_STRIPS 8
#define NUM_SEGMENTS 9

#define LIGHT_OFF 0
#define LIGHT_ON 1

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

int16_t state = LIGHT_OFF;
int16_t brightness = 50;

const uint16_t segment_bounds[] = { 0, 94, 142, 179, 196, 225, 242, 279, 360, 490 };

struct {
	uint8_t state = LIGHT_ON;
	CRGB color = CRGB::Yellow;
} segments[NUM_SEGMENTS];


MyMessage status_msg(CHILD_ID_BRIGHTNESS, V_STATUS);
MyMessage brightness_msg(CHILD_ID_BRIGHTNESS, V_PERCENTAGE);
MyMessage rgb_msg(CHILD_ID_SEG_1, V_RGB);

void setup()
{
	LEDS.addLeds<WS2811_PORTD, NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
}

void presentation()
{
    sendSketchInfo(SN, SV);

	present(CHILD_ID_BRIGHTNESS, S_DIMMER, "brightness");

	for (uint8_t i = 0; i < sizeof(segments); i++) {
		present(CHILD_ID_SEG_1 + i, S_RGB_LIGHT);
	}

	//present(CHILD_ID_IR, S_IR, "IR receiver");
}

void loop()
{
	static bool first_message_sent = false;
	if (first_message_sent == false) {
		update_all();
		send_all();
		LEDS.show();
		first_message_sent = true;
	}
}

void receive(const MyMessage &message)
{
	if (message.sensor == CHILD_ID_BRIGHTNESS) {
		if (message.type == V_STATUS) {
			state = message.getBool();
			update_brightness();
			send_status();
		}

		if (message.type == V_PERCENTAGE) {
			brightness = message.getInt();
			update_brightness();
			send_brightness();
		}
	}

	if (message.sensor >= CHILD_ID_SEG_1 && message.sensor < CHILD_ID_SEG_1 + sizeof(segments)) {
		uint8_t i = message.sensor - CHILD_ID_SEG_1;

		if (message.type == V_STATUS) {
			segments[i].state = message.getBool();
			update_segment(i);
			send_segment_status(i);
		}

		if (message.type == V_RGB) {
			String hexstring = message.getString();
			segments[i].color = strtol(&hexstring[0], NULL, 16);
			update_segment(i);
			send_segment_color(i);
		}
	}

	LEDS.show();
}

void update_all()
{
	update_brightness();

	for (uint8_t i = 0; i < sizeof(segments); i++) {
		update_segment(i);
	}
}

void update_brightness()
{
	if (state == LIGHT_ON) {
		LEDS.setBrightness(brightness * 255 / 100);
	} else {
		LEDS.setBrightness(0);
	}
}

void update_segment(uint8_t i)
{
	if (segments[i].state == LIGHT_ON) {
		fill_solid(&leds[segment_bounds[i]], segment_bounds[i + 1] - segment_bounds[i], segments[i].color);
	} else {
		fill_solid(&leds[segment_bounds[i]], segment_bounds[i + 1] - segment_bounds[i], CRGB::Black);
	}
}

void send_all()
{
	send_status();
	send_brightness();

	for (uint8_t i = 0; i < sizeof(segments); i++) {
		send_segment_status(i);
		send_segment_color(i);
	}
}

void send_status()
{
	status_msg.sensor = CHILD_ID_BRIGHTNESS;
	send(status_msg.set(state));
}

void send_brightness()
{
	send(brightness_msg.set(brightness));
}

void send_segment_status(uint8_t i)
{
	status_msg.sensor = CHILD_ID_SEG_1 + i;
	send(status_msg.set(segments[i].state));
}

void send_segment_color(uint8_t i)
{
	rgb_msg.sensor = CHILD_ID_SEG_1 + i;
	send(rgb_msg.set(String(segments[i].color, HEX)));
}
