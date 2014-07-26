/*
 * MotorAdapter.h
 *
 * Abstract Interface for all concrete adapters. Keeping this as simple as possible,
 * so for now the main method we need implemented is move().
 *
 *  Created on: Jul 24, 2014
 *      Author: Konstantin Gredeskoul
 *
 *  (c) 2014 All rights reserved.  Please see LICENSE.
 */

#ifndef BACKSEATDRIVER_IMOTORADAPTER_H_
#define BACKSEATDRIVER_IMOTORADAPTER_H_

class BackSeatDriver_IMotorAdapter {
public:
	virtual ~BackSeatDriver_IMotorAdapter() = 0;

	// attach/initialize as necessary
	virtual void attach() = 0;

	// detach/release/etc.
	virtual void detach() = 0;

	// move motors by providing speed (in % of max) for left set of wheels,
	// and right set of wheels. Negative means backwards. Zero for both
	// means stop.
	virtual void move(
			signed short leftSpeedPercent,
			signed short rightSpeedPercent) = 0;
private:
};


inline BackSeatDriver_IMotorAdapter::~BackSeatDriver_IMotorAdapter()
{
}

#endif /* BACKSEATDRIVER_IMOTORADAPTER_H_ */
