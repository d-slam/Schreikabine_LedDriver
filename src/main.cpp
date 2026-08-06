#include <Arduino.h>
#include <FastLED.h>
#include <math.h>

#define DBG(x) Serial.println(x)

#define NUM_LED 20
#define DATA_PIN 4

CRGB leds[NUM_LED];

const int micPin = 34;		// adc pin

float getRMS()
{
	const int samples = 1000;

	// brauchmor net...
	// long sum = 0;
	// // Mittelwert bestimmen (DC Offset)
	// for (int i = 0; i < samples; i++) { sum += analogRead(micPin); }
	// float offset = sum / (float)samples;

	// RMS berechnen
	float sumSquares = 0;

	for (int i = 0; i < samples; i++) {
		float value = analogRead(micPin); //- offset
		sumSquares += value * value;
	}
	return sqrt(sumSquares / samples);
}

float calibration = 43.42;	// aus der formel

void setup()
{
	Serial.begin(9600);
	DBG("Serial online!");

	FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LED);
}

void loop()
{
	float rms = getRMS();

	float dbspl = 20.0 * log10(rms) + calibration;

	// Serial.print("RMS: ");
	// Serial.print(rms);

	// Serial.print(" dB SPL: ");
	// Serial.println(dbspl);

	// delay(500);
}

