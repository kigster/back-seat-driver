/*
 * MotorAdapter.h
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
	virtual void attach() = 0;
	virtual void detach() = 0;
	virtual void move(signed short speedPercent) = 0;
	virtual void move(signed short leftSpeedPercent, signed short rightSpeedPercent) = 0;
private:
};


inline IMotorAdapter::~IMotorAdapter()
{
}

#endif /* IMOTORADAPTER_H_ */
