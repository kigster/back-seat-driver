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

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

typedef void(*maneuverCallback)(void);

#include <Servo.h>

class ServoMaster {
public:
	ServoMaster(unsigned short leftPin, unsigned short rightPin);

	void attach();
	void detach();
 	void goForward(unsigned short speedPercent);
 	void goForward(unsigned short speedPercent,
 			unsigned int durationMs,
 			maneuverCallback callback);
	void goBackward(unsigned short speedPercent);
	void goBackward(unsigned short speedPercent,
			unsigned int durationMs,
			maneuverCallback callback);
	void turn(int angle,
			maneuverCallback callback);
	void stop();
	bool isMoving();
	bool isManeuvering();
private:
	unsigned short _leftPin, _rightPin;
	signed short _currentSpeedPercent; // positive = forward, negative = backward
	bool _isManeuvering; // used for timed maneuvers
	unsigned long _maneuverStartMs, _maneuverDurationMs;
	maneuverCallback _maneuverCallback;

	Servo _left;
	Servo _right;

	int speedToMicroseconds(int speedPercent);
	void moveAtCurrentSpeed();
	void checkManeuveringState();
	void startManeuverTimer(unsigned int durationMs, maneuverCallback callback);
};

#endif /* ServoMaster_H */
