ServoMaster Library
===================

This library provides a convenient API to programmatically drive an autonomous vehicle, 
this particular one uses two Servo motors setup opposite each other (and thus to move forward, bot needs to rotate the two Servos in the opposite direction).

For an example hardware see DIY kit called "Parallax Arduino Robot Shield" 
available here: [http://www.parallax.com/product/323](http://www.parallax.com/product/323)

It is also an eternal learning quest for the author, who only started tinkering with Arduino 
at the end of June 2014. Therefore please keep in mind this library is not written by an 
Aruino expert, although the author does have an extensive software development background in 
other languages/platforms. 

Any suggestions or modifications are welcome, will be considered, and discussed in issues or pull requests.

## License

MIT.  See LICENSE file for more details.

## Introduction

### Concept

Imagine an autonomous robot, driving itself around, sensing and reacting to the environment 
around it. You can imagine yourself shouting robots command, as you see it approach a table: 
"Stop, turn left and see if it's any better".  For any robot to be autonomous this logic must also
be implemented in code. But the imagined scenario creates a clear boundary which separates 
an _instructional algorithm_ that avoids obstacles and determines what the robot does next,
from the _robot movement library_, which simply knows how to spin robot's wheels to achieve a move or a turn.

### Non-Blocking

This library is an attempt to provide a simple non-blocking robot command library based
on two servo motors.  The example provided, however, includes a simple instructional algorithm
also. 

Nowhere in the library is the ```delay()``` function called, and so the program flow is never 
paused.  The client of the library is expected to provided _callback_ functions to be executed
at the end of a given maneuver, such as a turn.  The client is also required to periodically 
call ```roboto->isManeuvering()``` to ensure all callbacks have a chance to execute and clear.

Library therefore supports:

* Duration-based maneuvers, "go forward for half a second"
* Unlimited moves, such as "go forward" indefinitely

#### Duration and Callbacks

Duratation based API calls, such as ```forward(speed, duration, callback)``` allow giving robot
instructions that will stop the robot after the duration time passes (defined in milliseconds 
from the start of the maneuver).  

As soon as the time period passes, the client's call to ```isManeuvering()``` will trigger
the callback (if provided). This is a "hook" mechanism, that allows executing additional steps
at the each maneuver, without having to wait using a ```delay()```. 

To remain within design goals of the library, the client should not perform any blocking itself, 
as doing this will allow multiple similar libraries to operate concurrently controlling multiple 
robot arms at the same time.
 
### Moving Forward, Backward and Speed

The speed value passed into the APIs provided by the library are expected to be always positive, 
and expressed in percent % of the total max speed of the servos.

```c++
	robot.forward(100); // puts motors into 100% speed, forward motion, and immediatey return
```

or

```c++
   // go backwards 
   robot.backwards(50, 1000, &turnAround);
   
   ....
   
   // somewhere else in the code
   
   // wait for the any manuevers to finish
   if (!robot.isManeuvering())  { ... }
```

In arguments to the converted to Servo's microseconds PWM timing. In addition we apply ArcTan() 
function to transform speed and provide more linear response between speed and RPMs, compared to 
when specifying microseconds directly. 

### Example

In this example the client uses Sonar sensor to detect objects ahead. If an object is found,
the robot turns left (-)45 degrees, and then immediatey checks again for distance.  If the distance
to the objects ahead is farther than the previous reading, it stays on this route and keeps moving. 
Otherwise it rotates 90 degrees, to now be at +45 degrees to the original direction. If that 
direction isn't better than the front, it turns for additional 135 degrees, making it a full 180'
degrees from the original direction. 

This algorighm is setup in a just a few lines of code using C/C++ style function pointers used
as callbacks at the end of each movement.

```c++
// Define the two pins used by the two Servos attached to the wheels (expected to be attached
// in an opposite direction to each other
ServoMaster robot = ServoMaster(13,12);

void setup()
{
	robot.attach();
}

void loop() 
{
    // The most important check below serves two key purposes:
    // 
    //   1. make sure that any existing manuevering that may be happening should be finished,
    //      and if so stop the robot and execute manuever's callbacks()
    //   2. avoids doing any instructions until the robot is out of the manuever.  This, of course,
    //      could be optional, and many things can be done even while the robot is manuevering.
    //      
     
	if (!bot.isManeuvering()) {
	    
	    // this is the default motion
		bot.goForward(100);
		
		// check distance to objects ahead
		spaceAhead = detectSpaceAhead();			
		if (spaceAhead < 50) { // if under < 50cm start manuevering
			// turn left 45 degrees, and when done call the checkLeft() function.
			robot.turn(-45, &checkLeft);
		}
	}
}

void checkLeft() {
	int spaceAfterTurn = spaceAhead();
	if (spaceAfterTurn < spaceAhead)
		bot.turn(90, &checkRight);
}

void checkRight() {
	int spaceAfterTurn = spaceAhead();
	if (spaceAfterTurn < spaceAhead)
		bot.turn(135, NULL);
}

```

### Working Model

I assembled this model in several hours, but added the Ultrasound distance sensor.  The sensor seems 
to be very narrow path focused. 

![Parallax Arduino Robot](examples/obstacle_avoidance_bot.jpg)

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

## Author

Konstantin Gredeskoul, @kig, http://github.com/kigster
