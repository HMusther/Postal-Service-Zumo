*NOTE* The code was tested on a zumo robot without front blade and
has issues working on other zumo robots. This was discussed with module team
and is fine, as I will demonstrate with the same zumo.

Zumo # ZU_32

INSTRUCTIONS FOR SETUP:
1. Open Arduino IDE.
2. Open "Latest.ino" sketch found in the git repository at "Postal-Service-Zuno/Latest/Latest.ino".
3. Connect the zumo to the computer system.
4. Upload the sketch to the zumo robot.

USAGE:
1. Place zumo at start point of the maze. (Works better when left line sensor is close to a black line).
2. Press "A" on the zumo robot.

Upon detecting an object, the zumo will sound a noise, and briefly pause until the object is gone.
The zumo will then continue to navigate the maze.

REFERENCES:
https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/BorderDetect/BorderDetect.ino
https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineAndProximitySensors/LineAndProximitySensors.ino
