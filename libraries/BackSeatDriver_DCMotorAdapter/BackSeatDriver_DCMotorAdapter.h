/*
 * BackSeatDriver_DCMotorAdapter.h
 *
 * Adapter for DC Motors using Adafruit MotorShield.
 *
 *  Created on: Jul 25, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */


#ifndef BackSeatDriver_DCMotorAdapter_H
#define BackSeatDriver_DCMotorAdapter_H

#define MAX_MOTORS 6

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <BackSeatDriver_IMotorAdapter.h>
#include <Adafruit_MotorShield.h>

class BackSeatDriver_DCMotorAdapter : public BackSeatDriver_IMotorAdapter {
public:

	// to configure DC motors, create an array as below, that lists motors
	// starting with front left, going down the left side, then back up the right
	// side (kind of like legs on the IC chip).  Passing negative value will
	// reverse that particular motor for all operations, so you can easily
	// configure this mapping regardless how your motors are wired up.
	//
	// signed short motorLayout[] = { 3, 4, 2, -1 };
	// BackSeatDriver_DCMotorAdapter adapter(4, motorLayout);
	//
	// In the above example motor 3 is left front, and motor 1 is right front,
	// but motor 1 was wired with reverse polarity so negative number compensates.

	BackSeatDriver_DCMotorAdapter(uint8_t numMotors, signed short motorMap[]);
	virtual ~BackSeatDriver_DCMotorAdapter();
	virtual void attach();
	virtual void detach();
	virtual void move(signed short leftSpeedPercent, signed short rightSpeedPercent);
private:
	signed short _motorMap[MAX_MOTORS];
	uint8_t _numMotors;
	Adafruit_DCMotor *_motors[MAX_MOTORS];
	Adafruit_MotorShield _AFMS;
	unsigned short speedFromPercentToValue(signed short speedPercent);
	bool isMotorReversed(uint8_t motor);
	void configureMotor(uint8_t index, signed short speedPercent);
};

#endif /* BackSeatDriver_DCMotorAdapter_H */
