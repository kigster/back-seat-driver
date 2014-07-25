/*
 * BackSeatDriver.cpp
 *
 *  Created on: Jul 16, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#include "BackSeatDriver.h"
#include <math.h>

BackSeatDriver::BackSeatDriver(IMotorAdapter *adapter) {
	_adapter = adapter;
	_debug = false;
	_initMs = millis();
	_lastDebugMs = 0;
	stop();
}

void BackSeatDriver::attach() {
	_adapter->attach();
}

void BackSeatDriver::detach() {
	_adapter->detach();
}

void BackSeatDriver::moveAtCurrentSpeed() {
	_adapter->move(_currentSpeedPercent, _currentSpeedPercent);
}

void BackSeatDriver::goForward(uint8_t speedPercent) {
	if (_currentSpeedPercent == ((signed short) speedPercent)) return;
	if (_debug) {
		sprintf(_logBuffer, "goForward(%3d), currentSpeed = %3d", speedPercent, _currentSpeedPercent); log();
	}
	_currentSpeedPercent = (signed short) speedPercent;
	moveAtCurrentSpeed();
}

void BackSeatDriver::goBackward(uint8_t speedPercent) {
	if (_currentSpeedPercent == ((signed short) speedPercent)) return;
	if (_debug) {
		sprintf(_logBuffer, "goBackward(%d)", speedPercent); log();
	}

	_currentSpeedPercent = -((signed short) speedPercent);
	moveAtCurrentSpeed();
}

void BackSeatDriver::goForward(uint8_t speedPercent,
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

void BackSeatDriver::goBackward(uint8_t speedPercent,
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

void BackSeatDriver::turn(int angle, maneuverCallback callback) {
	if (_debug) {
		sprintf(_logBuffer, "turn(%d, callback=%s)",
				angle,
				(callback == NULL ? "no" : "yes"));
		log();
	}

	stop();
	signed short speed = 100 * (angle == 0 ? 1 : abs(angle) / angle);
	_adapter->move(speed, -speed);
	startManeuverTimer(abs(angle) * 7, callback);
}

void BackSeatDriver::stop() {
	if (_debug) {
		sprintf(_logBuffer, "stop()");
		log();
	}
	_currentSpeedPercent = 0;
	stopManeuverTimer();
	moveAtCurrentSpeed();
}

bool BackSeatDriver::isMoving() {
	return (_currentSpeedPercent != 0);
}


bool BackSeatDriver::isManeuvering() {
	checkManeuveringState();
	return _maneuver.running;
}

void BackSeatDriver::debug(bool debugEnabled) {
	_debug = debugEnabled;
}

//________________________________________________________________________________________
//
// Private

void BackSeatDriver::stopManeuverTimer() {
	_maneuver.running = false;
	_maneuver.durationMs = 0;
	_maneuver.startMs = 0;
}

void BackSeatDriver::checkManeuveringState() {
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

void BackSeatDriver::startManeuverTimer(unsigned int durationMs,
		maneuverCallback callback) {
	_maneuver.callback = callback;
	_maneuver.startMs = millis();
	_maneuver.durationMs = durationMs;
	_maneuver.running = true;
}

void BackSeatDriver::log() {
	char millisSince[15];
	unsigned long ms = millis();
	if (ms - _lastDebugMs > SERVO_MIN_DEBUG_LOG_FREQ) {
		_lastDebugMs = ms;

		sprintf(millisSince, "%10d\t", (int)(_lastDebugMs - _initMs) );
		Serial.print(millisSince);
		Serial.println(_logBuffer);
	}
}
