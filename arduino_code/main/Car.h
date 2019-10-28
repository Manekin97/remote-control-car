#ifndef CAR_H
#define CAR_H

#include <ServoTimer2.h>
#include "UltraSonicSensor.h"
#include "constants.h"
#include <Arduino.h>

class Car {
  public:
    //  The constructor, it initializes pin modes,
    //  creates sensor object and attaches servo to its control pin
    Car();

    //  This function tells the car how to drive
    void drive();

    //  Control mode getter
    CONTROL_MODE getControlMode();

    //  This function executes the command
    void handleCommand(String command);
  private:
    //  Global variable used for storing the control mode
    CONTROL_MODE controlMode = REMOTE;

    //  Ultra sonic sensor object
    UltraSonicSensor *sensor;

    //  Servo control object
    ServoTimer2 servo;

    //  Method to control car movement
    void move(CAR_ACTION action, byte speed);

    //  Control mode setter
    void setControlMode(CONTROL_MODE mode);

    //  This function parses the command string and returns an array of values
    byte* getValuesFromString(String string);

    //  This function makes the car bypass an obstacle on the left side
    void bypassObstacleOnTheLeftSide();

    //  This function makes the car bypass an obstacle on the right side
    void bypassObstacleOnTheRightSide();

    //   This function checks for obstacle in the desired direction and returns the distance from it
    unsigned int checkForObstacles(DIRECTION direction);
};

#endif
