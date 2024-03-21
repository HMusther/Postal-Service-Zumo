#include <Wire.h>
#include <Zumo32U4.h>

// Zumo32U4 Components
Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

// #### Constants ####
// Threshold for line sensors. Lower the number the more sensitive the sensors will be.
#define QTR_THRESHOLD     800 // ms
#define REVERSE_SPEED     100  
#define TURN_SPEED        250
#define FORWARD_SPEED     50
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     50  // ms
#define NUM_SENSORS 5
// ####

// enum assigning index in lineSensorValues
enum sensors {
  Left = 0,
  MiddleLeft = 1,
  MiddleRight = 3,
  Right = 4
};

// Line sensor values
unsigned int lineSensorValues[NUM_SENSORS];

// Wait for input from user to start.
void waitForButtonAndCountDown()
{
  buttonA.waitForButton();

  ledYellow(0);

  // Play audible countdown.
  for (int i = 0; i < 2; i++)
  {
    buzzer.playNote(NOTE_G(3), 200, 15);
    delay(500);
  }
  delay(500);
  buzzer.playNote(NOTE_G(4), 500, 15);
}

// Method to read sensors.
bool ReadLineSensor(sensors s){
  lineSensors.read(lineSensorValues);
  return lineSensorValues[s] > QTR_THRESHOLD;
}

// Manouver: Turn Zumo right.

void TurnRight(int durationMultiplier = 1){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

}

// Manouver: Turn Zumo left.

void TurnLeft(int durationMultiplier = 1){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION * durationMultiplier);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}


// Move the zumo in the opposite direction of the line. 
// Breaks once the sensors do not detect the line.
void TurnUntilSensorFalse(sensors s1, sensors s2){

  while (ReadLineSensor(s1) || ReadLineSensor(s2)){
      if (s1 == Left){
          motors.setLeftSpeed(TURN_SPEED);
          motors.setRightSpeed(-TURN_SPEED);
      }
      else
      {
        motors.setLeftSpeed(-TURN_SPEED);
        motors.setRightSpeed(TURN_SPEED);
      }
  }

  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}


// Determine the manouver/action to take based on the line readings.
void RecalculateDirection(){
  bool leftSensor = ReadLineSensor(Left);
  bool middleLeftSensor = ReadLineSensor(MiddleLeft);
  bool middleRightSensor = ReadLineSensor(MiddleRight);
  bool rightSensor = ReadLineSensor(Right);

  // Happens when hitting line at 90 degree angle.
  // Turn left to avoid this.
  if (leftSensor && rightSensor){
    TurnLeft();
    TurnLeft();
  }
  // Track on the right side is over line. Turn towards left to get back on track.
  else if (middleRightSensor && !rightSensor){
    TurnUntilSensorFalse(Right, MiddleRight);
  }
  // Track on the left side is over the line. Turn towards right to get back on track.
  else if (middleLeftSensor && !leftSensor){
    TurnUntilSensorFalse(Left, MiddleLeft);
  }
  // Left side sensors detect line, move towards right.
  else if ((leftSensor || middleLeftSensor) && !rightSensor){
      TurnUntilSensorFalse(Left, MiddleLeft);
  }
  // Right side sensors detct line, move towards left.
  else if (!leftSensor && (rightSensor || middleRightSensor)){
      TurnUntilSensorFalse(Right, MiddleRight);
  }
  // Previous cases not true, Move to the left to allow for correction in next call.
  else if (leftSensor || rightSensor || middleLeftSensor || middleRightSensor){
    TurnUntilSensorFalse(Left, MiddleLeft);
  }
  // Move forwards, no lines have been detected.
  else{
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }

}

// Scan for objects in front of the zumo using proximity sensors.
// 
void ScanForObject(){

  proxSensors.read();

  if (proxSensors.countsFrontWithLeftLeds() >= 6 || proxSensors.countsFrontWithRightLeds() >= 6)
  {
    motors.setSpeeds(0,0);
    buzzer.playNote(NOTE_G(4), 500, 15);
    delay(2000);
  }
}

// Initialise sensors and wait for input.
void setup()
{
  lineSensors.initFiveSensors();
  proxSensors.initThreeSensors();
  proxSensors.setPeriod(15);

  waitForButtonAndCountDown();

}


void loop()
{
  // Wait for start instruction
  if (buttonA.isPressed())
  {
    // If button is pressed, stop and wait for another press to
    // go again.
    motors.setSpeeds(0, 0);
    buttonA.waitForRelease();
    waitForButtonAndCountDown();
  }

  // Scan for object (house) before moving.
  ScanForObject();

  // Determine next action for zumo.
  RecalculateDirection();

}


