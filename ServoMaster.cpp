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

ServoMaster::ServoMaster(unsigned short leftPin, unsigned short rightPin) {
	_leftPin = leftPin;
	_rightPin = rightPin;
	stop();
}

void ServoMaster::attach() {
	_left.attach(_leftPin);
	_right.attach(_rightPin);
}

void ServoMaster::detach() {
	_left.detach();
	_right.detach();
}


void ServoMaster::moveAtCurrentSpeed() {
	_left.writeMicroseconds(convertSpeedPercentToMicroseconds(_currentSpeedPercent));
	_right.writeMicroseconds(convertSpeedPercentToMicroseconds(-_currentSpeedPercent));
}

void ServoMaster::goForward(unsigned short speedPercent) {
	if (_currentSpeedPercent == (signed short) speedPercent) return;
	_currentSpeedPercent = speedPercent;
	moveAtCurrentSpeed();
}

void ServoMaster::goBackward(unsigned short speedPercent) {
	if (_currentSpeedPercent == (signed short) -speedPercent) return;
	_currentSpeedPercent = -speedPercent;
	moveAtCurrentSpeed();
}

void ServoMaster::goForward(unsigned short speedPercent,
		unsigned int durationMs,
		maneuverCallback callback) {
	startManeuverTimer(durationMs, callback);
	goForward(speedPercent);
}

void ServoMaster::goBackward(unsigned short speedPercent,
		unsigned int durationMs,
		maneuverCallback callback) {
	startManeuverTimer(durationMs, callback);
	goBackward(speedPercent);
}

void ServoMaster::turn(int angle, maneuverCallback callback) {
	stop();
	signed short speed = 100 * (angle == 0 ? 1 : abs(angle) / angle);
	_left.writeMicroseconds(convertSpeedPercentToMicroseconds(speed));
	_right.writeMicroseconds(convertSpeedPercentToMicroseconds(speed));
	startManeuverTimer(abs(angle) * 8, callback);
}

void ServoMaster::stop() {
	_currentSpeedPercent = 0;
	stopManeuverTimer();
	moveAtCurrentSpeed();
}

bool ServoMaster::isMoving() {
	return (_currentSpeedPercent != 0);
}


bool ServoMaster::isManeuvering() {
	checkManeuveringState();
	return _maneuver.running;
}

//________________________________________________________________________________________
//
// Private

void ServoMaster::stopManeuverTimer() {
	_maneuver.running = false;
	_maneuver.durationMs = 0;
	_maneuver.startMs = 0;
}

void ServoMaster::checkManeuveringState() {
	if (_maneuver.running && millis() - _maneuver.startMs > _maneuver.durationMs) {
		stop();

		maneuverCallback oldCallback = _maneuver.callback;
		_maneuver.callback = NULL;

		// call the callback (if defined) using previously saved poiner
		// start another maneuver, and yet reset _maneuverCallback
		// if no new maneuver was started.
		if (oldCallback != NULL) oldCallback();
	}
}

void ServoMaster::startManeuverTimer(unsigned int durationMs,
		maneuverCallback callback) {
	_maneuver.callback = callback;
	_maneuver.startMs = millis();
	_maneuver.durationMs = durationMs;
	_maneuver.running = true;
}

// speed can be between -100 and +100
int ServoMaster::convertSpeedPercentToMicroseconds(int speedPercent) {
	int value = (int) (
			// 1500 +
			(SERVO_MIN_MS + SERVO_HALF_RANGE_MS) +
			// +/- 200
			SERVO_HALF_RANGE_MS *
			// this ranges -1..1 for speedPercent between 0 and 100
			atan((double) (1.55 * speedPercent / 100.0)));
	return value;
}
