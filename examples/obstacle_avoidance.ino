/*
 * This example uses HC-SR04 distance sensor to detect obstacles and change course.
 *
 * Created on: Jul 16, 2014
 * Author: Konstantin Gredeskoul
 *
 * © 2014 All rights reserved.  Please see LICENSE.
 *
 */

#include <NewPing.h>

#define TRIGGER_PIN  5   // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     6   // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#include "Libraries/Util/Util.h"
#include <ServoMaster.h>

Util utility;
ServoMaster controller = ServoMaster(13, 12);

static unsigned int spaceAhead = 0, spaceAfterTurn;

unsigned int spaceInFront() {
	unsigned int value = sonar.ping() / US_ROUNDTRIP_CM;
	return (value == 0) ? MAX_DISTANCE : value;
}

void adjustDirection() {
	Serial.print("adjusting direction! distance is ");
	Serial.println(spaceAhead);
	controller.stop();
	controller.turn(45);

	spaceAfterTurn = spaceInFront();
	if (spaceAfterTurn > spaceAhead)
		return;

	controller.turn(-90);
	spaceAfterTurn = spaceInFront();
	if (spaceAfterTurn > spaceAhead) {
		return;
	}

	controller.turn(-120);
}

void setup() {
	Serial.begin(9600);
	utility.playResetSound(4);
	controller.begin();
	srand(millis());
}

void loop() {
	controller.forward(100, 100);
	spaceAhead = spaceInFront();
	if (spaceAhead > 0 && spaceAhead < 50) {
		adjustDirection();
	}

}

