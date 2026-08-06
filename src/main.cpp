#include <Arduino.h>

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
	Serial.begin(115200);
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