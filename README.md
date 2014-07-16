ServoMaster Library
===================

This library provides high level abstraction for using two-servo motors to power a self-driving robot,
similar to Parallax Arduino Robot Shield available here: http://www.parallax.com/product/32335

## Introduction

The library provides a simple and convenient abstraction to make the robot go ```forward()``` or ```back()```,
or ```turn()``` a given number of degrees (positive meaning "right", negative "left").

It should work on any two-servo robot, but can be easily adapted to more servo motors.

### Speed

The speed is specified as a constant between -100 and 100 (negative being backward motion). This is internally
converted to Servo's microseconds PWM timing. In addition we apply ArcTan() function to transform speed and provide
more linear response between speed and RPMs, compared to when specifying microseconds directly.

### Example

```c++
// define two pins used by two Servos
ServoMaster controller = ServoMaster(13,12);

void setup()
{
	controller.begin();
}

void loop() 
{
	controller.forward(100, 500); // speed 100%, go for 500ms
	controller.stop();
	controller.turn(45);          // turn right 45 degrees
	controller.back(50,200)       // go backwards at 50% speed, for 200ms
	controller.stop();
	controller.turn(-90);
}
```

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Added some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request

## Author

Konstantin Gredeskoul, @kig, http://github.com/kigster
