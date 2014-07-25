/*
 * BackSeatDriver.h
 *
 *  Created on: Jul 16, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#ifndef BackSeatDriver_H
#define BackSeatDriver_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include <utility/IMotorAdapter.h>

#define LOG_BUFFER_LEN 50
#define MIN_DEBUG_LOG_FREQ 50

typedef void(*maneuverCallback)(void);

typedef struct maneuverStruct {
	unsigned long durationMs;
	unsigned long startMs;
	maneuverCallback callback;
	bool running;
} maneuver;


class BackSeatDriver {
public:
	BackSeatDriver(IMotorAdapter *adapter);

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
	void turn(int angle,
			maneuverCallback callback);
	void stop();
	bool isMoving();
	bool isManeuvering();
	void debug(bool debugEnabled);
private:
	IMotorAdapter *_adapter;
	// positive = forward, negative = backward
	signed short _currentSpeedPercent;
	maneuver _maneuver;
	char _logBuffer[LOG_BUFFER_LEN];
	unsigned long _initMs, _lastDebugMs;

	bool _debug;

	void moveAtCurrentSpeed();
	void checkManeuveringState();
	void startManeuverTimer(unsigned int durationMs, maneuverCallback callback);
	void stopManeuverTimer();
	void log(void);
};

#endif /* BackSeatDriver_H */
