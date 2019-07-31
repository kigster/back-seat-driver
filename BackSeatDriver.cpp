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

BackSeatDriver::BackSeatDriver(BackSeatDriver_IMotorAdapter *adapter) {
	_adapter = adapter;
	_debug = false;
	_initMs = millis();
	_lastDebugMs = 0;
	_turningSpeedPercent = _movingSpeedPercent = 100;
	_turningDelayCoefficient = 7; // multiplier for how long to wait for a turn.
	stop();
}

void BackSeatDriver::attach() {
	_adapter->attach();
}

void BackSeatDriver::detach() {
	_adapter->detach();
}

void BackSeatDriver::goForward(uint8_t speedPercent) {
	if (_currentSpeedPercent == ((signed short) speedPercent)) return;
	if (_debug) {
		sprintf(_logBuffer, "goForward(%3d), currentSpeed = %3d", speedPercent, _currentSpeedPercent); log();
	}
	_currentSpeedPercent = (signed short) constrain(speedPercent, 0, 100);
	moveAtCurrentSpeed();
}

void BackSeatDriver::goBackward(uint8_t speedPercent) {
	if (_currentSpeedPercent == ((signed short) speedPercent)) return;
	if (_debug) {
		sprintf(_logBuffer, "goBackward(%d)", speedPercent); log();
	}

	_currentSpeedPercent = -((signed short) constrain(speedPercent, 0, 100));
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

	startManeuverTimer(durationMs, MANEUVER_FORWARD, speedPercent, callback);
	goForward(speedPercent);
}

void BackSeatDriver::goBackward(uint8_t speedPercent,
		unsigned int durationMs,
		maneuverCallback callback) {

	if (_debug) {
		sprintf(_logBuffer, "goBackward(%d, %d, callback=%s)",
				speedPercent,
				durationMs,
				(callback == NULL ? "no" : "yes"));
		log();
	}
	startManeuverTimer(durationMs, MANEUVER_BACK, speedPercent, callback);
	goBackward(speedPercent);
}

void BackSeatDriver::turn(signed short angle, maneuverCallback callback) {
	if (_debug) {
		sprintf(_logBuffer, "turn(%d, callback=%s)",
				angle,
				(callback == NULL ? "no" : "yes"));
		log();
	}

	if (abs(_currentSpeedPercent) > 0)
		stop();

	signed short speed = 100 * (angle == 0 ? 1 : abs(angle) / angle);
	speed = (signed short) (1.0f * (float)speed * (float)_turningSpeedPercent / 100.0f);
	moveAtSpeed(speed, -speed, _turningSpeedPercent);
	startManeuverTimer(abs(angle) * _turningDelayCoefficient, MANEUVER_TURN, angle, callback);
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

void BackSeatDriver::setMovingSpeedPercent(unsigned short movingSpeedPercent) {
	if (movingSpeedPercent >= 0 && movingSpeedPercent <= 100) {
		_movingSpeedPercent = movingSpeedPercent;
	}
}
void BackSeatDriver::setTurningSpeedPercent(unsigned short turningSpeedPercent) {
	if (turningSpeedPercent >= 0 && turningSpeedPercent <= 100) {
		_turningSpeedPercent = turningSpeedPercent;
	}
}

void BackSeatDriver::setTurningDelayCoefficient(unsigned short turningDelayCoefficient) {
	_turningDelayCoefficient = turningDelayCoefficient;
}

//
// Private

void BackSeatDriver::moveAtSpeed(signed short leftPercent, signed short rightPercent, unsigned short adjustmentPercent) {
	leftPercent = (signed short) (1.0 * (float)(leftPercent) * adjustmentPercent / 100.0);
	rightPercent = (signed short) (1.0 * (float)(rightPercent) * adjustmentPercent / 100.0);
	_adapter->move(leftPercent, rightPercent);
}

void BackSeatDriver::moveAtCurrentSpeed() {
	moveAtSpeed(_currentSpeedPercent, _currentSpeedPercent, _movingSpeedPercent);
}


void BackSeatDriver::stopManeuverTimer() {
	_maneuver.running = false;
	_maneuver.durationMs = 0;
	_maneuver.startMs = 0;
	_maneuver.type = 0;
	_maneuver.parameter = 0;
	_maneuver.callback = NULL;
}

void BackSeatDriver::checkManeuveringState() {
	if (_maneuver.running && millis() - _maneuver.startMs > _maneuver.durationMs) {
		maneuver currentManeuver = _maneuver;  // copy maneuver so that _maneuver can be reset
		stop();

		// call the callback (if defined) using previously saved poiner
		// start another maneuver, and yet reset _maneuverCallback
		// if no new maneuver was started.
		if (currentManeuver.callback != NULL) {
			const char *callbackType;
			switch (currentManeuver.type) {
			case MANEUVER_BACK:
				callbackType = "backward";
				break;
			case MANEUVER_FORWARD:
				callbackType = "forward";
				break;
			case MANEUVER_TURN:
				callbackType = "turn";
				break;
			default:
				callbackType = "unknown";
			}

			if (_debug) { sprintf(_logBuffer, "running callback after %s(%d)", callbackType, currentManeuver.parameter); log(); }
			currentManeuver.callback(currentManeuver.type, currentManeuver.parameter);
		}
	}
}

void BackSeatDriver::startManeuverTimer(unsigned int durationMs, uint8_t type, signed short parameter, maneuverCallback callback) {
	_maneuver.callback = callback;
	_maneuver.startMs = millis();
	_maneuver.durationMs = durationMs;
	_maneuver.running = true;
	_maneuver.type = type;
	_maneuver.parameter = parameter;
}

void BackSeatDriver::logForward() {
	uint32_t ms = millis();
	if (ms - _lastDebugMs > MIN_DEBUG_LOG_FREQ) {
		_lastDebugMs = ms;
		log();
	}
}

void BackSeatDriver::log(const char *message) {
	if (strlen(message) < LOG_BUFFER_LEN) {
		sprintf(_logBuffer, "%s", message);
		log();
	}
}

void BackSeatDriver::log() {
	char millisSince[15];
	uint32_t ms = millis();
	_lastDebugMs = ms;
	sprintf(millisSince, "%10d\t", (int)(_lastDebugMs - _initMs) );
	Serial.print(millisSince);
	Serial.println(_logBuffer);
}
