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

enum Status {
  Searching = 0,
  Returning = 1
};
enum sensors {
  Left = 0,
  Right = 4
};

// Storing L (Left) F (Forward) R (Right) U (Turn Around)
char movements[1000];
int movementIndex = 0;

int wallBounceCount = 0;
int bounceCount = 0;

bool isStuck = false;
Status status = Searching;

// Change next line to this if you are using the older Zumo 32U4
// with a black and green LCD display:
// Zumo32U4LCD display;
Zumo32U4OLED display;

Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;


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

bool ReadLineSensor(sensors s){
  lineSensors.read(lineSensorValues);
  if (lineSensorValues[s] > QTR_THRESHOLD){
    return true;
  }else{
    return false;
  }
}

bool ReadProxSensor(sensors s){
  proxSensors.read();
  if (s == Left){
    return proxSensors.countsFrontWithLeftLeds() >= 2;
  }
  else if(s == Right){
    return proxSensors.countsFrontWithRightLeds() >= 2;
  }
}

void TurnAround(){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(-TURN_SPEED * 2, TURN_SPEED * 2);
      delay(TURN_DURATION * 5);
      movementIndex++;
      movements[movementIndex] = 'U';
}

void TurnRight(int durationMultiplier = 1){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      movementIndex++;
      movements[movementIndex] = 'R';
}

void TurnLeft(int durationMultiplier = 1){
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION * durationMultiplier);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      movementIndex++;
      movements[movementIndex] = 'L';
}

void LongTurnLeft(){
      buzzer.playNote(NOTE_G(3), 200, 15);
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION * 2);
      motors.setSpeeds(-TURN_SPEED * 4, TURN_SPEED * 4);
      movementIndex += 2;
      movements[movementIndex] = 'L';
      movements[movementIndex - 1] = 'L';
}

void LongTurnRight(){
      buzzer.playNote(NOTE_G(3), 200, 15);
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION * 2);
      motors.setSpeeds(TURN_SPEED * 4, -TURN_SPEED * 4);
      movementIndex += 2;
      movements[movementIndex] = 'R';
      movements[movementIndex - 1] = 'R';
}

void MoveForwards(){
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
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

  if (status == Searching){
    // if (ReadProxSensor(Left) && ReadProxSensor(Right)){
    //   TurnAround();
    //   return;
    // }
    // else if (ReadProxSensor(Left))
    // {
    //   TurnRight();
    //   return;
    // }
    // else if (ReadProxSensor(Right)){
    //   TurnLeft();
    //   return;
    // }
    
    if (ReadLineSensor(Left) && ReadLineSensor(Right)){
        TurnAround();
        wallBounceCount = 0;
    }
    else if (ReadLineSensor(Left))
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
    else if (ReadLineSensor(Right))
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
      MoveForwards();
    }
  }
  else
  {
    for (int i = sizeof(movements) - 1; i >= 0; i--){
      char movement = movements[i];

      switch (movement){
        case 'L':
          TurnRight();
          break;
        case 'R':
          TurnLeft();
          break;
        case 'U':
          TurnAround();
          break;
        case 'F':
          TurnAround();
          break;
        default:
          return;
      }
    }
  }
}


