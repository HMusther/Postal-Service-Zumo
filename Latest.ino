/** This example uses the Zumo's line sensors to detect the white
border around a sumo ring.  When the border is detected, it
backs up and turns. */

#include <Wire.h>
#include <Zumo32U4.h>




// This might need to be tuned for different lighting conditions,
// surfaces, etc.
#define QTR_THRESHOLD     1000  // microseconds

// These might need to be tuned for different motor types.
#define REVERSE_SPEED     100  // 0 is stopped, 400 is full speed
#define TURN_SPEED        100
#define FORWARD_SPEED     100
#define REVERSE_DURATION  200  // ms
#define TURN_DURATION     300  // ms

int wallBounceCount = 0;
bool isStuck = false;


enum sensors {
  Left = 0,
  Right = 4
};


// Change next line to this if you are using the older Zumo 32U4
// with a black and green LCD display:
// Zumo32U4LCD display;
Zumo32U4OLED display;

Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;

int bounceCount = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

void waitForButtonAndCountDown()
{
  ledYellow(1);
  display.clear();
  display.print(F("Press A"));

  buttonA.waitForButton();

  ledYellow(0);
  display.clear();

  // Play audible countdown.
  for (int i = 0; i < 1; i++)
  {
    delay(500);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(500);
  buzzer.playNote(NOTE_G(4), 500, 15);
  delay(500);
}

bool ReadSensor(sensors s){
  lineSensors.read(lineSensorValues);
  if (lineSensorValues[s] > QTR_THRESHOLD){
    return true;
  }else{
    return false;
  }
}

void TurnAround(){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(-TURN_SPEED * 2, TURN_SPEED * 2);
      delay(TURN_DURATION * 5);
}

void TurnRight(int durationMultiplier = 1){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}

void TurnLeft(int durationMultiplier = 1){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION * durationMultiplier);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}

void LongTurnLeft(){
      buzzer.playNote(NOTE_G(3), 200, 15);
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION * 2);
      motors.setSpeeds(-TURN_SPEED * 4, TURN_SPEED * 4);
}

void LongTurnRight(){
      buzzer.playNote(NOTE_G(3), 200, 15);
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION * 2);
      motors.setSpeeds(TURN_SPEED * 4, -TURN_SPEED * 4);
}



void setup()
{
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  display.clear();
  display.gotoXY(0, 0);

  lineSensors.initFiveSensors();
  proxSensors.initThreeSensors();


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

  if (ReadSensor(Left) && ReadSensor(Right)){
      TurnAround();
      wallBounceCount = 0;
  }
  else if (ReadSensor(Left))
  {
    wallBounceCount++;
    isStuck = wallBounceCount >= 5 ? true : false;

    if (isStuck){
        LongTurnRight();
    }

    if (!isStuck){
      TurnRight();
    }
    else
    {
      TurnRight(4);
      wallBounceCount = 0;

    }
  }
  else if (ReadSensor(Right))
  { 
    wallBounceCount++;
    isStuck = wallBounceCount >= 5 ? true : false;

    if (isStuck){
        LongTurnLeft();
    }

    // If rightmost sensor detects line, reverse and turn to the
    // left.
    if (!isStuck){
      TurnLeft();
    }
    else
    {
      TurnLeft(4);
      wallBounceCount = 0;
    }

  }
  else
  {
    // Otherwise, go straight.
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}


