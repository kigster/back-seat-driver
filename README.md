BackSeatDriver: Autonomous Vehicle Library for Arduino
====================================================

This library provides a convenient non-blocking command API to programmatically drive an
autonomous vehicle based on various motor configurations expressed as concrete adapters.

Supported adapters include:

 * _BackSeatDriver_TwoServoAdapter_ – is for cars with two Servo motors setup opposite each other. Therefore to move the robot forward (or backward), two Servos need to rotate in the opposite direction.
 * _BackSeatDriver_DCMotorAdapter_ – is for cars with 2 or 4 DC motors (still experimental).

### Library Features

* Intuitive and easy to read/use API
* Non-blocking duration-based maneuvers with and without time limit, i.e. "go forward for half a second, then do this..."
* Turn by angle
* Add your own adapters and use the same API and callbacks

### Non-Blocking Control

We do not use ```delay()``` function anywhere.  This means that the program flow is never paused. The client of the library is able to provide a _callback_ function to be executed at the end of a given maneuver, such as a turn. While the maneuver is executing, other operations may proceed.

As a trade-off, the client is required to periodically call ```robot->isManeuvering()``` function, to ensure that all callbacks have a chance to
execute and clear, and any maneuvers complete.  If this function is not called frequently enough, turns can go on for longer than required, and become inaccurate.
The library also does not user interrupts.

### Hardware Requirements

For an example hardware see DIY kit called "Parallax Arduino Robot Shield"
available here: [http://www.parallax.com/product/323](http://www.parallax.com/product/323)

Any Arduino card with 2 Servo motors attached would work.  For most Servo motors that can move the vehicle you would need a decent power supply.  The robot above uses 5 x 1.5V AA batteries for a total of 7.5V and about 1Amp.  A dedicated motor shield such as Adafruit Motor Shield V2, would be an excellent choice, but there are many others.


## Usage

### Moving Forward, Backward

The speed value passed into the APIs provided by the library are expected to be always positive, and expressed in percent % of the total max speed of the motors.

```c++
// puts motors into 100% speed, forward motion,
// and immediately return from the function
robot.goForward(100);
```

### Callbacks

Alertnatively, a version with callbacks can be used.  Callbacks are functions of type ```maneuverCallback```, which is defined as:

```c++
typedef void(*maneuverCallback)(uint8_t type, signed short parameter);
```

When a turn or a movement ends, the callback is automatically called, and two parameters are passed into it. First parameter is the
type of the previous maneuver: MANEUVER_TURN or MANEUVER_BACK or MANEUVER_FORWARD.  Depending on the type, parameter can be the angle
of the turn (negative for left, or positive for right), or the speed of the movement (always positive).  This way callbacks can
perform customized actions depending on the previous data.

```c++
// go backwards @ 50% speed, for 1 second, and then call
// turnAround() local function defined somewhere in this context
robot.goBackward(50, 1000, &turnAround);


void turnAround(uint8_t type, short int parameter) {
   short int angle = parameter;
   if (type == MANEUVER_TURN && angle < 0) {
       // perform another maneuver
   }
}
....

// somewhere else in the code

// wait for the any maneuvers to finish
if (!robot.isManeuvering())  { ... }
```

### Examples


#### Initializing a 2-Servo Robot


```c++
// load specific Adapter for our motors
#include <BackSeatDriver_TwoServoAdapter.h>

// now load the main library
#include <BackSeatDriver.h>

// initialize the adapter with two pins assigned to the two servos
BackSeatDriver_TwoServoAdapter adapter(13, 12);

// intialize BackSeatDriver itself, passing it the driver.
BackSeatDriver robot(&adapter);

// now we can ask our robot to move...
robot.goForward(100); // move forward at 100% speed
```

#### Initializing a 4-DC Motor Robot

When mapping DC motors, each will rotate in a specific direction depending on
where the positive and negative charge was attached. If you accidentally connected
one of the motors in reverse, then simply pass that motor number as negative integer, and it will work as expected.

In the below example we declar that motor 3 is front left, 4 is back left, 2 is back right (but reversed) and 1 is front right (but also reversed). This is a very powerful and simple way to avoid having to resolder or re-wire motors after assembly :)

```c++
// load specific adapter we are using
#include <BackSeatDriver_DCMotorAdapter.h>

// now load the main library
#include <BackSeatDriver.h>

signed short motorLayout[] = { 3, 4, -2, -1 };

BackSeatDriver_DCMotorAdapter adapter(4, motorLayout);
BackSeatDriver racer(&adapter);

racer.goForward(50); // move forward at 50% speed
```

#### Avoiding Obstacles using Sonar Sensor

In this example we use a Sonar sensor to detect objects ahead.  The algorithm is setup in a just a few lines of code using C/C++ style function pointers used as callbacks at the end of each maneuver, but provide for a pretty effective
obstacle avoidance strategy (but albeit a random direction).

```c++
static BackSeatDriver_TwoServoAdapter adapter = BackSeatDriver_TwoServoAdapter(13, 12);
static BackSeatDriver robot = BackSeatDriver(&adapter);

void setup()
{
    robot.attach();
}

void loop()
{
    // The most important check below serves two key purposes:
    //
    //   1. make sure that any existing maneuvering that may be happening should be
    //      finished, and if so stop the robot and execute maneuver's callbacks()
    //
    //   2. avoids doing any instructions until the robot is out of the maneuver.
    //      This could be optional, as many things can be done during the time
    //      robot is maneuvering, perhaps with other arms or sensors.
    //

    if (!bot.isManeuvering()) {

        // this is the default motion
        robot.goForward(100);

        // check distance to objects ahead
        spaceAhead = detectSpaceAhead();
        if (spaceAhead < 50) { // if under < 50cm start manuevering
            // turn left 45 degrees, and when done call the checkLeft() function.
            robot.turn(-45, &checkLeft);
        }
    }
}

void checkLeft(uint8_t type, short int parameter) {
    int spaceAfterTurn = spaceAhead();
    if (spaceAfterTurn < spaceAhead)
        robot.turn(90, &checkRight);
}

void checkRight(uint8_t type, short int parameter) {
    int spaceAfterTurn = spaceAhead();
    if (spaceAfterTurn < spaceAhead)
        robot.turn(135, NULL);
}
```

#### Adjusting Movement Speed

Not all bots are made equal, and sometimes you'll want to adjust movement and turning speed.

By default, setting movement speed of 100% sets the robot at max speed. This may not always
be desired.  For this purpose the following call exists:

```c++
robot.setMovingSpeedPercent(80); // multiply all speed arguments from now on by 0.8
```

This configures a global coefficient, which is applied to all arguments of forward and backward
movement.  For example,

```c++
robot.setMovingSpeedPercent(80); // set global speed adjustment coefficient
robot.goForward(50);             // set current speed at 50% of max
```

For a DC motor example, where 255 would be the max value sent to the motor, the actual value
will be computed as ```255 * 0.8 * 0.5 = 102```.  Typically you would call ```setMovingSpeedPercent()```
once in the ```setup()``` to configure your specific robot.

#### Adjusting Turning Speed and Delay Coefficient

When turns are requested, robot is sent a signal to rotate left/right wheels in the opposite direction.
There are two main parameters controlling this:

 1. how fast should the wheels rotate during a turn?
 2. how long should the turn last?

The default for how fast wheels rotate is 100%, which if very often not what you want.  Call
```setTurningSpeedPercent()``` to a value less than 100 to adjust this.  Please note that this value is
independent of the ```setMovingSpeedPercent()``` and is always computed based on the max speed of the robot,
not the adjusted speed.

The default for how long the turn lasts is computed based on a simple linear formula: ```angle x turningDelayCoefficient ```
where ```angle``` is in degrees (say 90, or 180).

So if the ```turningDelayCoefficient``` is set to 10, then 90-degree turn will take 900ms.

The default for this value is 7.

Future version of the library may allow each adapter to offer a custom way of computing turn delay and speed,
if a linear relationship is not appropriate (for example smaller terms take longer to complete, while larger turns
are faster).

#### Debugging

Calling the ```debug()``` function with enable debug mode:

```c++
    robot.debug(true);
```

In this mode library sends information about what the robot is doing back to the
Serial port. Here is an example below (first column is milliseconds since program start).

```
     13395	goForward(100), currentSpeed =  95
     16358	goForward( 71), currentSpeed = 100
     16411	goForward( 73), currentSpeed =  71
     16462	goForward( 72), currentSpeed =  73
     16577	goForward(100), currentSpeed =  72
     16690	turn(-45, callback=yes)
     17052	stop()
```

### Working Model

Here is a model I assembled, with an added Ultrasound distance sensor.
The sensor seems to be pretty focused on a narrow path.

#### Servo Bot (Parallax Arduino Kit)

![Parallax Arduino Robot](bot_servo.jpg)

#### DC Motor Bot (Custom built, Adafruit Motor Shield)

![DC Motor Robot with Adafruit Motor Shield](bot_dc.jpg)

#### Another DC Motor Bot (dx.com frame, Adafruit Motor Shield)

![DC Motor Robot with Adafruit Motor Shield](bot_dc_chaseroni.jpg)

Note the 5A fuse attached to the body. After one of my wires smoked, I decided to use fuses
on all powered vehicles.  I'd rather burn a fuse than my house :)

### Disclaimer and Invitation to Collaborate

This project is also an eternal learning quest for the author, who only started tinkering with Arduino at the end of June 2014. Therefore please keep in mind that this library is not written by an Arduino expert, although the author does have an extensive software development background in other languages.

Any suggestions or modifications are welcome, and will be considered, discussed and decided on in the issues or pull requests.


## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

## Author

Konstantin Gredeskoul, @kig, http://github.com/kigster

## License

MIT.  See LICENSE file for more details.

