/*
 * ServoMaster.h
 *
 *  Created on: Jul 16, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#ifndef ServoMaster_H
#define ServoMaster_H

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <Servo.h>

class ServoMaster {
public:
	ServoMaster(uint8_t leftPin, uint8_t rightPin);
	void begin();
	void forward(uint8_t speed, int duration);
	void back(uint8_t speed, int duration);
	void turn(int angle);
	void stop();
private:
	uint8_t _leftPin, _rightPin;
	Servo _left;
	Servo _right;
	int speedToMicroseconds(int speedPercent);
};

#endif /* ServoMaster_H */
