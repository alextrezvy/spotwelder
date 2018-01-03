/*
 * ShiftDisplay1Reg.cpp
 *
 *  Created on: 26 окт. 2017 г.
 *      Author: user
 */

#include "ShiftDisplay1Reg.h"

ShiftDisplay1Reg::ShiftDisplay1Reg(const byte aData, const byte aClock, const byte aLatch) {
	pinData = aData;
	pinClock = aClock;
	pinLatch = aLatch;
	value = 0;
	position = 0;
	pinMode(pinData, OUTPUT);
	pinMode(pinClock, OUTPUT);
	pinMode(pinLatch, OUTPUT);
}

void ShiftDisplay1Reg::setValue(byte aValue) {
	value = aValue;
}

/*
 * Displaying one digit per call.
 */
void ShiftDisplay1Reg::display() {
	byte digit = (position == 0) ? value % 10 : (value / 10) % 10;
	byte bits = DIGITS[digit];
	// if not first digit, then turn the least bit to 1 (activating the second LED unit)
	if (position > 0) {
		bits = bits | 1;
	}
	digitalWrite(pinLatch, LOW);
	shiftOut(pinData, pinClock, LSBFIRST, bits);
	digitalWrite(pinLatch, HIGH);
	position++;
	if (position > 1)
		position = 0;
}


