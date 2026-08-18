#include <Arduino.h>
#include <FastLED.h>
// #include <math.h>

#define DBG(x) Serial.println(x)

#define NUM_LED 160
#define DATA_PIN 4
CRGB leds[NUM_LED];

const int pinInputSignal = A7;

float getRMS()
{
	const int samples = 256;

	float sum = 0.0f;
	float values[samples];

	for (int i = 0; i < samples; i++)
	{
		values[i] = analogRead(pinInputSignal);
		sum += values[i];
	}

	float offset = sum / samples;

	float sumSquares = 0.0f;

	for (int i = 0; i < samples; i++)
	{
		float value = values[i] - offset;
		sumSquares += value * value;
	}

	return sqrtf(sumSquares / samples);
}

// Kalibrierwert mit dB-Meter bestimmen
float calibration = 33.42; //dB=20⋅log10​(RMS)+K => K=dB−20⋅log10​(RMS)	: db(leveldBMeter), RMS(rms)

// Glättung
float dbSmooth = 0;

void setup()
{
	Serial.begin(9600);
	DBG("Serial online!");

	FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LED);
	FastLED.setBrightness(255);

	FastLED.clear();
	FastLED.show();
}

void loop()
{
	float rms = getRMS();

	if (rms < 0.001f)	rms = 0.001f;
	if (!isfinite(rms) || rms <= 0.0f)	{ return; }

	// RMS -> dB SPL
	float dbspl = 20.0f * log10(rms) + calibration;

	if (!isfinite(dbspl))	{ return; }			// iwos mochen wenn die checks failn...

	// Glätten
	// dbSmooth = dbSmooth * 0.7f + dbspl * 0.3f;
	if (dbspl > dbSmooth) 	{ dbSmooth += (dbspl - dbSmooth) * 0.4f; }	// attack
	else					{ dbSmooth += (dbspl - dbSmooth) * 0.1f; }	// release

	// Wertebereich für LED Balken
	float dbMin = 40.0f;
	float dbMax = 120.0f;

	int ledCount = map(dbSmooth, dbMin, dbMax, 0, NUM_LED);		// achtung: map castet als long, helper für float wäre supper
	ledCount = constrain(ledCount, 0, NUM_LED);

	// Alle LEDs aus
	fill_solid(leds, NUM_LED, CRGB::Black);

	// LEDs einschalten
	for (int i = 0; i < ledCount; i++)
	{
		float percent = (float)i / NUM_LED;

		if (percent < 0.5f)			leds[i] = CRGB::Green;
		else if (percent < 0.8f)	leds[i] = CRGB::Yellow;
		else						leds[i] = CRGB::Red;
	}

	// EINMAL pro Loop
	FastLED.show();

	Serial.print("RMS: ");
	Serial.print(rms);

	Serial.print("  dB SPL: ");
	Serial.println(dbSmooth);
}