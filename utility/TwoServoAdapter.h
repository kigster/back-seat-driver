/*
 * TwoServoAdapter.h
 *
 *  Created on: Jul 24, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#ifndef TWOSERVOADAPTER_H_
#define TWOSERVOADAPTER_H_

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

#include <utility/IMotorAdapter.h>
#include <Servo.h>

class TwoServoAdapter  : public IMotorAdapter {
public:
	TwoServoAdapter(uint8_t leftPin, uint8_t rightPin);
	virtual ~TwoServoAdapter();
	virtual void attach();
	virtual void detach();
	virtual void move(signed short leftSpeedPercent, signed short rightSpeedPercent);
private:
	uint8_t _leftPin, _rightPin;
	int convertSpeedPercentToMicroseconds(signed short speedPercentWithSign);
	Servo _left;
	Servo _right;
};

#endif /* TWOSERVOADAPTER_H_ */
