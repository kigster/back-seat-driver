/*
 * ServoMaster.cpp
 *
 *  Created on: Jul 16, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#include "ServoMaster.h"
#include <math.h>

ServoMaster::ServoMaster(uint8_t leftPin, uint8_t rightPin) {
	_leftPin = leftPin;
	_rightPin = rightPin;
}

void ServoMaster::begin() {
	_left.attach(_leftPin);
	_right.attach(_rightPin);
}

void ServoMaster::forward(uint8_t speed, int duration) {
	_left.writeMicroseconds(speedToMicroseconds(speed));
	_right.writeMicroseconds(speedToMicroseconds(-speed));
	delay(duration);
}

void ServoMaster::back(uint8_t speed, int duration) {
	_left.writeMicroseconds(speedToMicroseconds(-speed));
	_right.writeMicroseconds(speedToMicroseconds(speed));
	delay(duration);

}

void ServoMaster::turn(int angle) {
	short sign = angle == 0 ? 1 : abs(angle) / angle;
	int speed = 100;
	_left.writeMicroseconds(speedToMicroseconds(speed * sign));
	_right.writeMicroseconds(speedToMicroseconds(speed * sign));
	delay(abs(angle) * 6);
}

void ServoMaster::stop() {
	_left.writeMicroseconds(speedToMicroseconds(0));
	_right.writeMicroseconds(speedToMicroseconds(0));
	delay(20);
}

// speed can be between -100 and +100
int ServoMaster::speedToMicroseconds(int speedPercent) {
	int value = (int) (1500 + // that's the middle
			150.0 * // plus/minus 200
					atan((double) (1.55 * speedPercent / 100.0))); // this makes curve more linear
	Serial.print("Converted Speed ");
	Serial.print(speedPercent);
	Serial.print(" to Microseconds ");
	Serial.println(value);
	return value;
}
