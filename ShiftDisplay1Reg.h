/*
 * ShiftDisplay1Reg.h
 *
 *  Created on: 26.10.2017
 *  Author: Alex Trezvy
 *  The class encapsulates an output of a 2 digit decimal value to a 7-segment LED display
 *  which is connected with 1 pcs 74HC595 shift register.
 *  The last bit of the register defines the position of the value to be displayed at a time.
 */

#ifndef SHIFTDISPLAY1REG_H_
#define SHIFTDISPLAY1REG_H_
#include <Arduino.h>

/* Lowest significant bit goes to the highest (max) bit of the register  */
const byte DIGITS[] = {
		B00000010,
		B10011110,
		B00100100,
		B00001100,
		B10011000,
		B01001000,
		B01000000,
		B00011110,
		B00000000,
		B00001000
};

class ShiftDisplay1Reg {
private:
	byte pinData;
	byte pinClock;
	byte pinLatch;
	byte value;
	byte position; // of the value to display on the next iteration

public:
	ShiftDisplay1Reg(const byte pinData, const byte pinClock, const byte pinLatch);
	void setValue(byte value);
	void display();
};

#endif /* SHIFTDISPLAY1REG_H_ */
