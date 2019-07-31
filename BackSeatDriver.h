/*
 * BackSeatDriver.h
 *
 *  Created on: Jul 16, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#ifndef BACKSEATDRIVER_H
#define BACKSEATDRIVER_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "BackSeatDriver_IMotorAdapter.h"

#define LOG_BUFFER_LEN 50
#define MIN_DEBUG_LOG_FREQ  50

#define MANEUVER_BACK		 1
#define MANEUVER_FORWARD 	 2
#define MANEUVER_TURN 		 3

typedef void(*maneuverCallback)(uint8_t type, signed short parameter);

typedef struct maneuverStruct {
	unsigned long durationMs;
	unsigned long startMs;
	maneuverCallback callback;
	bool running;
	uint8_t type;       	// MANEUVER_TURN, etc.
	signed short parameter; // turn angle, etc.
} maneuver;


class BackSeatDriver {
public:
	BackSeatDriver(BackSeatDriver_IMotorAdapter *adapter);

	void attach();
	void detach();
 	void goForward(uint8_t speedPercent);
 	void goForward(uint8_t speedPercent,
 			unsigned int durationMs,
 			maneuverCallback callback);
	void goBackward(uint8_t speedPercent);
	void goBackward(uint8_t speedPercent,
			unsigned int durationMs,
			maneuverCallback callback);
	void turn(signed short angle,
			maneuverCallback callback);
	void stop();
	bool isMoving();
	bool isManeuvering();
	void debug(bool debugEnabled);

	// set to 100 to go full speed; anything less than 100 to cap max speed
	// and reduce the speed range.
	void setMovingSpeedPercent(unsigned short movingSpeedPercent);

	// speed of the turn as a percentage of max speed
	void setTurningSpeedPercent(unsigned short turningSpeedPercent);

	// multiplier determining how long turns last.  Turn duration is determined by
	// multiplying angle (in degrees) by this coefficient, eg, for 90' it's
	// 90 x turningDelayCoefficient (ms).  Default is 7.
	void setTurningDelayCoefficient(unsigned short turningDelayCoefficient);
	void log(const
	char *);
private:
	unsigned short
		_movingSpeedPercent,
		_turningSpeedPercent,
		_turningDelayCoefficient;

	BackSeatDriver_IMotorAdapter *_adapter;
	// positive = forward, negative = backward
	signed short _currentSpeedPercent;
	maneuver _maneuver;
	char _logBuffer[LOG_BUFFER_LEN];
	uint32_t _initMs, _lastDebugMs;

	bool _debug;

	void moveAtSpeed(
			signed short leftPercent,
			signed short rightPercent,
			unsigned short adjustmentPercent);
	void moveAtCurrentSpeed();
	void checkManeuveringState();
	void startManeuverTimer(unsigned int durationMs, uint8_t type, signed short parameter, maneuverCallback callback);
	void stopManeuverTimer();
	void log(void);
	void logForward(void);
};

#endif /* BACKSEATDRIVER_H */
