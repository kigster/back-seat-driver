/*
 * BackSeatDriver_DCMotorAdapter.cpp
 *
 *  Created on: Jul 25, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */


#include "BackSeatDriver_DCMotorAdapter.h"

BackSeatDriver_DCMotorAdapter::BackSeatDriver_DCMotorAdapter(uint8_t numMotors, signed short motorMap[]) {
	_numMotors = numMotors;
	for (short i = 0; i < _numMotors; i++) {
		_motorMap[i] = motorMap[i];
	}
}

BackSeatDriver_DCMotorAdapter::~BackSeatDriver_DCMotorAdapter() {
	//
}

void BackSeatDriver_DCMotorAdapter::attach() {
	_AFMS = Adafruit_MotorShield();
	_AFMS.begin();
	for (short i = 0; i < _numMotors; i++) {
		_motors[i] = _AFMS.getMotor(abs(_motorMap[i]));
	}
}

void BackSeatDriver_DCMotorAdapter::detach() {
	for (short i = 0; i < _numMotors; i++) {
		_motors[i]->setSpeed(0);
	}
}

void BackSeatDriver_DCMotorAdapter::move(signed short speedPercentLeft, signed short speedPercentRight) {
	for (uint8_t i = 0; i < _numMotors / 2; i++) {
		configureMotor(i, speedPercentLeft);
	}
	for (uint8_t i = _numMotors / 2; i < _numMotors; i++) {
		configureMotor(i, speedPercentRight);
	}
}

unsigned short BackSeatDriver_DCMotorAdapter::speedFromPercentToValue(signed short speedPercent) {
	return constrain(255 * abs(1.0 * speedPercent / 100.0), 0, 255);
}

bool BackSeatDriver_DCMotorAdapter::isMotorReversed(uint8_t motor) {
	if (motor >= _numMotors) {
		return false;
	}
	return (_motorMap[motor] < 0);
}

void BackSeatDriver_DCMotorAdapter::configureMotor(uint8_t index, signed short speedPercent) {
	_motors[index]->setSpeed(speedFromPercentToValue(speedPercent));
	if (abs(speedPercent) < 10) {  // too small values don't give out enough current to spin wheels.
		_motors[index]->run(BRAKE);
		_motors[index]->run(RELEASE);
	} else {
		if (isMotorReversed(index) xor (speedPercent < 0)) {
			_motors[index]->run(BACKWARD);
		} else {
			_motors[index]->run(FORWARD);
		}
	}
}


