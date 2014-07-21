/*
 * ServoMaster.h
 *
 *  Created on: Jul 16, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#ifndef ServoMaster_H
#define ServoMaster_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#define SERVO_VELOCITY_TRIG
#define SERVO_LOG_BUFFER_LEN 50
#define SERVO_MIN_DEBUG_LOG_FREQ 50

typedef void(*maneuverCallback)(void);

typedef struct maneuverStruct {
	unsigned long durationMs;
	unsigned long startMs;
	maneuverCallback callback;
	bool running;
} maneuver;

#ifndef SERVO_MIN_MS
#define SERVO_MIN_MS 1300
#endif

#ifndef SERVO_MAX_MS
#define SERVO_MAX_MS 1700
#endif

// this would be 200 for the above values.
#ifndef SERVO_HALF_RANGE_MS
#define SERVO_HALF_RANGE_MS (SERVO_MAX_MS - SERVO_MIN_MS) / 2
#endif


#include <Servo.h>

class ServoMaster {
public:
	ServoMaster(uint8_t leftPin, uint8_t rightPin);

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
	uint8_t _leftPin, _rightPin;
	signed short _currentSpeedPercent;
	// positive = forward, negative = backward
	maneuver _maneuver;
	char _logBuffer[SERVO_LOG_BUFFER_LEN];
	unsigned long _initMs, _lastDebugMs;

	Servo _left;
	Servo _right;

	bool _debug;

	int convertSpeedPercentToMicroseconds(signed short speedPercentWithSign);
	void moveAtCurrentSpeed();
	void checkManeuveringState();
	void startManeuverTimer(unsigned int durationMs, maneuverCallback callback);
	void stopManeuverTimer();
	void log(void);
};

#endif /* ServoMaster_H */
