/**
 * Precise single pulse generator by Alex Trezvy.
 * Generates one pulse with a fixed duration.
 * Duration can be set with buttons from 10 ms upto 990 ms.
 * The value is displayed on 2-digit 7-segment LED display.
 * New value is stored in EEPPROM.
 */

#include <Arduino.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include "ShiftDisplay1Reg.h"

#define FW_VERSION 0.1

// hardware config
const byte pinBtnR = 2;
const byte pinBtnL = 3;
const byte pinBtnF = 4;
const byte pinDS   = 5;
const byte pinClk  = 6;
const byte pinStor = 7;
const byte pinFire = 8;
// workflow params
const int minFireDuration = 1;
const int maxFireDuration = 99;
const int intervalPRC = 500; // ms
const int intervalRC  = 100; // ms
const int intervalRest = 500;// ms
// FSM states
const byte ST_IDLE = 0;
const byte ST_PRC  = 1; // Prepare for Rapid Change of delay
const byte ST_RC   = 2; // Rapid Change of delay
const byte ST_AF   = 3; // Active Fire
const byte ST_REST = 4; // Rest after fire

// Array of bits for each character. Used 7-seg LED with common anode.
const byte charBits[] = {
		B00000010,
		B10011110,
		B00100100,
		B00001100,
		B10011000,
		B01001000,
		B01000000,
		B00011110,
		B00000000,
		B00001000};

Bounce btnR; // right button
Bounce btnL; // left button
Bounce btnF; // fire button
ShiftDisplay1Reg disp(pinDS, pinClk, pinStor); // output device
byte currentState = ST_IDLE;
byte currentDuration = minFireDuration; // Delay of the arc. 1 unit = 10 ms.
boolean isIncrement = true;
unsigned long timestampPRC = 0;
unsigned long timestampLastChange = 0;
unsigned long timestampFire = 0;
unsigned int address = 0; // where to save the duration.

void setup() {
	Serial.begin(115200);
	Serial.print("Precise single pulse generator. Firmware version: ");
	Serial.println(FW_VERSION);
	btnR.attach(pinBtnR, INPUT_PULLUP);
	btnL.attach(pinBtnL, INPUT_PULLUP);
	btnF.attach(pinBtnF, INPUT_PULLUP);
	pinMode(pinFire, OUTPUT);
	// Retrieving previously saved duration and remember it's address.
	for (int i = 0; i < EEPROM.length(); i++) {
		byte val = EEPROM.read(i);
		if (val != 0xFF) {
			currentDuration = val;
			address = i;
			EEPROM.write(address, 0xFF);
			address++;
			if (address == EEPROM.length()) {
				address = 0;
			}
			EEPROM.write(address, currentDuration);
			break;
		}
	}
	disp.setValue(currentDuration);
}

void durationIncrement() {
	if (currentDuration < maxFireDuration)
		currentDuration++;
	disp.setValue(currentDuration);
}

void durationDecrement() {
	if (currentDuration > minFireDuration)
		currentDuration--;
	disp.setValue(currentDuration);
}

void loop() {
	btnR.update();
	btnL.update();
	btnF.update();

	switch (currentState) {
		case ST_IDLE:
			if (btnR.fell() || btnL.fell()) {
				currentState = ST_PRC;
				timestampPRC = millis();
				timestampLastChange = timestampPRC;
				if (btnR.fell()) {
					isIncrement = true;
					durationIncrement();
				} else {
					isIncrement = false;
					durationDecrement();
				}
			} else if (btnF.fell()) {
				currentState = ST_AF;
				EEPROM.update(address, currentDuration);
				timestampFire = millis();
				digitalWrite(pinFire, HIGH);
			}
			break;
		case ST_PRC:
			if (millis() - timestampPRC > intervalPRC) {
				currentState = ST_RC;
			} else {
				if (btnR.rose() || btnL.rose())
					currentState = ST_IDLE;
			}
			break;
		case ST_RC:
			if (btnR.rose() || btnL.rose()) {
				currentState = ST_IDLE;
			} else if (millis() - timestampLastChange > intervalRC) {
				timestampLastChange = millis();
				isIncrement ? durationIncrement() : durationDecrement();
			}
			break;
		case ST_AF:
			if (millis() - timestampFire > 10*currentDuration) {
				currentState = ST_REST;
				digitalWrite(pinFire, LOW);
			}
			break;
		case ST_REST:
			if (millis() - timestampFire > 10*currentDuration + intervalRest) {
				currentState = ST_IDLE;
			}
			break;
	}

	disp.display();
}

