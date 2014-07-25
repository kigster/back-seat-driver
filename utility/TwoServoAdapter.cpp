/*
 * TwoServoAdapter.cpp
 *
 *  Created on: Jul 24, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#include <utility/TwoServoAdapter.h>
#include <math.h>

TwoServoAdapter::TwoServoAdapter(uint8_t leftPin, uint8_t rightPin) {
	_leftPin = leftPin;
	_rightPin = rightPin;
}

TwoServoAdapter::~TwoServoAdapter() {
	//
}

void TwoServoAdapter::attach() {
	_left.attach(_leftPin);
	_right.attach(_rightPin);
}

void TwoServoAdapter::detach() {
	_left.detach();
	_right.detach();
}

void TwoServoAdapter::move(signed short leftSpeed, signed short rightSpeed) {
	_left.writeMicroseconds(convertSpeedPercentToMicroseconds(leftSpeed));
	_right.writeMicroseconds(convertSpeedPercentToMicroseconds(-rightSpeed));
}

void TwoServoAdapter::move(signed short speed) {
	move(speed, speed);
}

// speed can be between -100 and +100
int TwoServoAdapter::convertSpeedPercentToMicroseconds(signed short speedPercentWithSign) {
	int value = 0;
#ifdef SERVO_VELOCITY_LINEAR
	// linear formula, goes between 1500 (0%) and 1600 (100%).
	value = (int) (
			SERVO_MIN_MS + SERVO_HALF_RANGE_MS +
			(int) ((0.5) * SERVO_HALF_RANGE_MS * (float) speedPercentWithSign / 100.0);
#else
	// default is apply tan() to flatten out response curve
	value = (int) (
			SERVO_MIN_MS + SERVO_HALF_RANGE_MS +
			(SERVO_HALF_RANGE_MS / 2) * tan((double)speedPercentWithSign / 100.0) / 1.55);
#endif

#ifdef DEBUG_VELOCITY
	if (_debug) {
		sprintf(_logBuffer, "speed value is %d for %d %%, range is %d", value, speedPercentWithSign,
				SERVO_HALF_RANGE_MS); log();
	}
#endif
	return value;
}

