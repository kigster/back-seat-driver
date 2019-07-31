/*
 * TwoServoAdapter.cpp
 *
 *  Created on: Jul 24, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#include <BackSeatDriver_TwoServoAdapter.h>
#include <math.h>

BackSeatDriver_TwoServoAdapter::BackSeatDriver_TwoServoAdapter(uint8_t leftPin, uint8_t rightPin) {
	_leftPin = leftPin;
	_rightPin = rightPin;
}

BackSeatDriver_TwoServoAdapter::~BackSeatDriver_TwoServoAdapter() {
	//
}

void BackSeatDriver_TwoServoAdapter::attach() {
	_left.attach(_leftPin);
	_right.attach(_rightPin);
}

void BackSeatDriver_TwoServoAdapter::detach() {
	_left.detach();
	_right.detach();
}

void BackSeatDriver_TwoServoAdapter::move(signed short leftSpeed, signed short rightSpeed) {
	_left.writeMicroseconds(constrain(convertSpeedPercentToMicroseconds(leftSpeed), SERVO_MIN_MS, SERVO_MAX_MS));
	_right.writeMicroseconds(constrain(convertSpeedPercentToMicroseconds(-rightSpeed), SERVO_MIN_MS, SERVO_MAX_MS));
}

// speed can be between -100 and +100
int BackSeatDriver_TwoServoAdapter::convertSpeedPercentToMicroseconds(signed short speedPercentWithSign) {
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

