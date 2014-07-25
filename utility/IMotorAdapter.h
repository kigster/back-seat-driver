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

#ifndef IMOTORADAPTER_H_
#define IMOTORADAPTER_H_

class IMotorAdapter {
public:
	virtual ~IMotorAdapter() = 0;

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


inline IMotorAdapter::~IMotorAdapter()
{
}

#endif /* IMOTORADAPTER_H_ */
