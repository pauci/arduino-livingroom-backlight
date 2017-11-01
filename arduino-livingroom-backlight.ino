#include <FastLED.h>

#define MY_GATEWAY_SERIAL

#include <MySensors.h>

#define CHILD_ID_RGB 1
#define CHILD_ID_IR 2

#define SN "LivingroomGW"
#define SV "1.0"


#define NUM_LEDS 470

#define NUM_LEDS_PER_STRIP 64
#define NUM_STRIPS 8

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

CRGB color = CRGB::Yellow;

#define LIGHT_OFF 0
#define LIGHT_ON 1

int16_t last_state = LIGHT_ON;

MyMessage status_msg(CHILD_ID_RGB, V_STATUS);
MyMessage rgb_msg(CHILD_ID_RGB, V_RGB);

void setup()
{
	LEDS.addLeds<WS2811_PORTD, NUM_STRIPS>(leds, NUM_LEDS_PER_STRIP);
	LEDS.setBrightness(255);
	LEDS.showColor(color);

	request(CHILD_ID_RGB, V_RGB);
}

void presentation()
{
	sendSketchInfo(SN, SV);
  present(CHILD_ID_IR, S_IR, "IR receiver");
	present(CHILD_ID_RGB, S_RGB_LIGHT, "RGB light");
}

void loop()
{
	static bool first_message_sent = false;
	if (first_message_sent == false) {
		send_rgb_message();
		send_status_message();
		first_message_sent = true;
	}
}

void receive(const MyMessage &message)
{
	if (message.type == V_RGB) {
		String hexstring = message.getString();
		color = strtol(&hexstring[0], NULL, 16);
		send_rgb_message();
	}
	if (message.type == V_STATUS) {
		last_state = message.getInt();
		send_status_message();
	}
	update_leds();
}

void update_leds()
{
	if (last_state == LIGHT_ON) {
		LEDS.showColor(color);
	} else {
		LEDS.clear(true);
	}
}

void send_rgb_message()
{
	send(rgb_msg.set(String(color, HEX)));
}

void send_status_message()
{
	if (last_state == LIGHT_OFF) {
		send(status_msg.set((int16_t)0));
	} else {
		send(status_msg.set((int16_t)1));
	}
}