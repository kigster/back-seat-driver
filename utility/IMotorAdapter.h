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
	virtual void attach();
	virtual void detach();
	virtual void move(signed short speedPercent);
	virtual void move(signed short leftSpeedPercent, signed short rightSpeedPercent);
private:
};

#endif /* IMOTORADAPTER_H_ */
