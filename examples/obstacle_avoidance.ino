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

#define TRIGGER_PIN  3   // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     2   // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#include "Libraries/Util/Util.h"
#include <ServoMaster.h>

Util utility;
ServoMaster bot = ServoMaster(10, 11);

const short int leftWhiskerPin = 9, rightWhiskerPin = 6;
const short int maxSpeed = 100;
short int speed = maxSpeed;
static unsigned int spaceAhead = 0, spaceAfterTurn;
static bool leftWhisk, rightWhisk = false;

static const unsigned int sonarCheckPeriodMs = 50; // don't check more often than that
static unsigned int lastSonarAtMs = 0;

//===============================================================

unsigned int spaceInFront() {
	unsigned int value = sonar.ping() / US_ROUNDTRIP_CM;
	return (value == 0) ? MAX_DISTANCE : value;
}

void adjustDirection() {
	turnLeft();
}

void turnLeft() {
	Serial.println("turn -45");
	bot.turn(-45, &checkLeft);
}

void checkLeft() {
	Serial.println("check left");
	spaceAfterTurn = spaceInFront();
	detectWhiskers(&leftWhisk, &rightWhisk);
	if (leftWhisk || spaceAfterTurn < spaceAhead)
		bot.turn(90, &checkRight);
}

void checkRight() {
	Serial.println("check right");
	spaceAfterTurn = spaceInFront();
	detectWhiskers(&leftWhisk, &rightWhisk);
	if (rightWhisk || spaceAfterTurn < spaceAhead)
		bot.turn(135, NULL);
}

void detectWhiskers(bool *left, bool *right) {
	*left = (digitalRead(leftWhiskerPin) == LOW);
	*right = (digitalRead(rightWhiskerPin) == LOW);
}

void hardRight() {
	Serial.println("turn 90");
	bot.turn(90, NULL);
}

void hardLeft() {
	Serial.println("turn -90");
	bot.turn(-90, NULL);
}

bool navigateWithWhiskers() {
	detectWhiskers(&leftWhisk, &rightWhisk);

	if (leftWhisk || rightWhisk) {
		bot.stop();
		bot.goBackward(speed,
				500,
				leftWhisk ? &hardRight : &hardLeft);
		return true;
	}
	return false;
}

void setup() {
	Serial.begin(9600);
	utility.playResetSound(4);

	bot.attach();

	srand(millis());

	pinMode(leftWhiskerPin, INPUT);
	pinMode(rightWhiskerPin, INPUT);
}

void loop() {

	if (!bot.isManeuvering()) {
		bot.goForward(speed);
		if (millis() > sonarCheckPeriodMs + lastSonarAtMs) {
			spaceAhead = spaceInFront();
			lastSonarAtMs = millis();
			if (spaceAhead < 50) {
				adjustDirection();
			}
		} else {
			navigateWithWhiskers();
		}
	}
}

