/*
 * This example uses HC-SR04 distance sensor to detect obstacles and change course.

 * Dependencies: Wire, ICP, NewPing, Servo
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
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters).
                         // Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#include <ServoMaster.h>
#include <ServoArm.h>

ServoMaster bot = ServoMaster(10, 11);
ServoArm arm = ServoArm(13, 12, -7);

const short int leftWhiskerPin = 9, rightWhiskerPin = 6;
const short int maxSpeed = 100;
short int speed = maxSpeed;
static unsigned int lastSpaceAhead = 0, spaceAfterTurn;
static bool leftWhisk, rightWhisk = false;

static const unsigned int sonarCheckPeriodMs = 50; // don't check more often than that
static unsigned int lastSonarAtMs = 0;

//_______________________________________________________________________
//
// Sonar Navigation

unsigned int spaceAhead() {
	unsigned int value = sonar.ping() / US_ROUNDTRIP_CM;
	return (value == 0) ? MAX_DISTANCE : value;
}

bool navigateWithSonar() {
	if (millis() > sonarCheckPeriodMs + lastSonarAtMs) {
		lastSpaceAhead = spaceAhead();
		lastSonarAtMs = millis();
		if (lastSpaceAhead < 50) {
			bot.turn(-45, &checkLeft);
			return true;
		}
	}
	return false;
}

void checkLeft() {
	spaceAfterTurn = spaceAhead();
	detectWhiskers(&leftWhisk, &rightWhisk);
	if (leftWhisk || spaceAfterTurn < lastSpaceAhead)
		bot.turn(90, &checkRight);
}

void checkRight() {
	spaceAfterTurn = spaceAhead();
	detectWhiskers(&leftWhisk, &rightWhisk);
	if (rightWhisk || spaceAfterTurn < lastSpaceAhead)
		bot.turn(135, NULL);
}

//_______________________________________________________________________
//
// Whisker Based Navigation

bool navigateWithWhiskers() {
	detectWhiskers(&leftWhisk, &rightWhisk);

	if (leftWhisk || rightWhisk) {
		bot.stop();
		bot.goBackward(speed, 500, leftWhisk ? &hardRight : &hardLeft);
		return true;
	}
	return false;
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

// Arduino API callbacks
//____________________________________________________________________

void setup() {
	srand(millis());
	Serial.begin(9600);

	bot.attach();

	pinMode(leftWhiskerPin, INPUT);
	pinMode(rightWhiskerPin, INPUT);
}

void loop() {
	if (!bot.isManeuvering()) {

		bot.goForward(speed);

		// call our navigation processors one by one, but as soon as one of them
		// starts maneuvering we skip the rest. If we bumped into whiskers, we sure
		// don't need sonar to tell us we have a problem :)
		navigateWithWhiskers() || navigateWithSonar() ; // || .....
	}
}

