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
	_isManeuvering = false;
	_maneuverCallback = NULL;
	_maneuverStartMs
		= _maneuverDurationMs
		=_currentSpeedPercent = 0;
}

void ServoMaster::attach() {
	_left.attach(_leftPin);
	_right.attach(_rightPin);
}

void ServoMaster::detach() {
	_left.detach();
	_right.detach();
}

void ServoMaster::goForward(unsigned short speedPercent) {
	if (_currentSpeedPercent == speedPercent) return;
	_currentSpeedPercent = speedPercent;
	moveAtCurrentSpeed();
}

void ServoMaster::goBackward(unsigned short speedPercent) {
	if (_currentSpeedPercent == -speedPercent) return;
	_currentSpeedPercent = -speedPercent;
	moveAtCurrentSpeed();
}

void ServoMaster::startManeuverTimer(unsigned int durationMs,
		maneuverCallback callback) {
	_maneuverStartMs = millis();
	_maneuverDurationMs = durationMs;
	_isManeuvering = true;
	_maneuverCallback = callback;
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

void ServoMaster::moveAtCurrentSpeed() {
	_left.writeMicroseconds(speedToMicroseconds(_currentSpeedPercent));
	_right.writeMicroseconds(speedToMicroseconds(-_currentSpeedPercent));
}

void ServoMaster::turn(int angle, maneuverCallback callback) {
	stop();
	signed short speed = 100 * (angle == 0 ? 1 : abs(angle) / angle);
	_left.writeMicroseconds(speedToMicroseconds(speed));
	_right.writeMicroseconds(speedToMicroseconds(speed));
	startManeuverTimer(abs(angle) * 8, callback);
}

void ServoMaster::stop() {
	_currentSpeedPercent = 0;
	_isManeuvering = false;
	_maneuverDurationMs = 0;
	_maneuverStartMs = 0;

	moveAtCurrentSpeed();
}

bool ServoMaster::isMoving() {
	return (_currentSpeedPercent != 0);
}

void ServoMaster::checkManeuveringState() {
	if (_isManeuvering && millis() - _maneuverStartMs > _maneuverDurationMs) {
		Serial.print("stop maneuvering, have callback? ");
		Serial.println(_maneuverCallback == NULL);
		stop();

		maneuverCallback oldCallback = _maneuverCallback;
		_maneuverCallback = NULL;

		// this "trick" allows us to call the callback which might
		// start another maneuver, and yet reset _maneuverCallback
		// if no new maneuver was started.
		if (oldCallback != NULL) oldCallback();
	}
}

bool ServoMaster::isManeuvering() {
	checkManeuveringState();
	return _isManeuvering;
}
// speed can be between -100 and +100
int ServoMaster::speedToMicroseconds(int speedPercent) {
	int value = (int) (1500 + // that's the middle
			200.0 * // plus/minus 200
					atan((double) (1.55 * speedPercent / 100.0))); // this makes curve more linear
//	Serial.print("Converted Speed ");
//	Serial.print(speedPercent);
//	Serial.print(" to Microseconds ");
//	Serial.println(value);
	return value;
}
