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
	_debug = false;
	_initMs = millis();
	_lastDebugMs = 0;
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

void ServoMaster::goForward(uint8_t speedPercent) {
	if (_currentSpeedPercent == ((signed short) speedPercent)) return;
	if (_debug) {
		sprintf(_logBuffer, "goForward(%3d), currentSpeed = %3d", speedPercent, _currentSpeedPercent); log();
	}
	_currentSpeedPercent = (signed short) speedPercent;
	moveAtCurrentSpeed();
}

void ServoMaster::goBackward(uint8_t speedPercent) {
	if (_currentSpeedPercent == ((signed short) speedPercent)) return;
	if (_debug) {
		sprintf(_logBuffer, "goBackward(%d)", speedPercent); log();
	}

	_currentSpeedPercent = -((signed short) speedPercent);
	moveAtCurrentSpeed();
}

void ServoMaster::goForward(uint8_t speedPercent,
		unsigned int durationMs,
		maneuverCallback callback) {

	if (_debug) {
		sprintf(_logBuffer, "goForward(%d, %d, callback=%s)",
				speedPercent,
				durationMs,
				(callback == NULL ? "no" : "yes"));
		log();
	}

	startManeuverTimer(durationMs, callback);
	goForward(speedPercent);
}

void ServoMaster::goBackward(uint8_t speedPercent,
		unsigned int durationMs,
		maneuverCallback callback) {

	if (_debug) {
		sprintf(_logBuffer, "goForward(%d, %d, callback=%s)",
				speedPercent,
				durationMs,
				(callback == NULL ? "no" : "yes"));
		log();
	}
	startManeuverTimer(durationMs, callback);
	goBackward(speedPercent);
}

void ServoMaster::turn(int angle, maneuverCallback callback) {
	if (_debug) {
		sprintf(_logBuffer, "turn(%d, callback=%s)",
				angle,
				(callback == NULL ? "no" : "yes"));
		log();
	}

	stop();
	signed short speed = 100 * (angle == 0 ? 1 : abs(angle) / angle);
	_left.writeMicroseconds(convertSpeedPercentToMicroseconds(speed));
	_right.writeMicroseconds(convertSpeedPercentToMicroseconds(speed));
	startManeuverTimer(abs(angle) * 8, callback);
}

void ServoMaster::stop() {
	if (_debug) {
		sprintf(_logBuffer, "stop()");
		log();
	}
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

void ServoMaster::debug(bool debugEnabled) {
	_debug = debugEnabled;
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
		if (oldCallback != NULL) {
			if (_debug) { sprintf(_logBuffer, "executing callback"); log(); }
			oldCallback();
		}
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
int ServoMaster::convertSpeedPercentToMicroseconds(signed short speedPercentWithSign) {
	int value = 0;
#ifdef SERVO_VELOCITY_LINEAR
	// linear formular, goes between 1500 (0%) and 1600 (100%).
	value = (int) (
			SERVO_MIN_MS + SERVO_HALF_RANGE_MS +
			(int) ((0.5) * SERVO_HALF_RANGE_MS * (float) speedPercentWithSign / 100.0);
#endif
#ifdef SERVO_VELOCITY_TRIG
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

void ServoMaster::log() {
	char millisSince[15];
	unsigned long ms = millis();
	if (ms - _lastDebugMs > SERVO_MIN_DEBUG_LOG_FREQ) {
		_lastDebugMs = ms;

		sprintf(millisSince, "%10d\t", (long)(_lastDebugMs - _initMs) );
		Serial.print(millisSince);
		Serial.println(_logBuffer);
	}
}
