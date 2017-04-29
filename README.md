# eBot example
Example program for using the eBot library written with PlatformIO, to be used as well with other boards.

## IDE
* PlatformIO IDE: used for developement
* Arduino IDE: copy content of main.cpp into Arduino IDE

## Needed Libraries
* [eBot.h](https://github.com/jipp/eBot)

## The following features are used
- [x] bluetooth remote control
- [x] different movements - automatic and manual mode
- [x] obstacle avoidance - automatic mode
- [ ] Line tracking

## The following features are in progress
* line tracking

## Info
* for obstacle avoidance Ultrasonic is used. This is only working for flat surfaces.
* the following commands can be used in any mode:
  * '-' - decreases speed
  * '+' - increase speed
  * 'M' - switch between manual and autonomous mode
  * 'c' - switch LED state
* the following commands can be used - manual mode:
  * 's' - stop
  * 'f' - forward
  * 'b' - backward
  * 'l' - turn left
  * 'r' - turn right
  * 'L' - rotate leftt
  * 'R' - rotate right

## Tested hardware
### Boards
* Elegoo Smart Robot Car V2.0
