#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

enum CAR_ACTION {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  STOP
};

//  Sensor direction enum
enum DIRECTION {
  FRONT_SIDE,
  RIGHT_SIDE,
  LEFT_SIDE
};

//  Control mode enum
enum CONTROL_MODE {
  REMOTE,
  AUTONOMOUS
};

//  AI algorithm enum
enum AI_ALGORITHM {
  SIMPLE,
  ADVANCED,
  COMPLEX
};

//  Motor control pins
#define ENABLE_LEFT_MOTOR   9
#define ENABLE_RIGHT_MOTOR  10
#define LEFT_MOTOR_IN_1     4
#define LEFT_MOTOR_IN_2     5
#define RIGHT_MOTOR_IN_3    6
#define RIGHT_MOTOR_IN_4    7

//  Sensor's pins
#define ECHO    12
#define TRIGGER 13

//  Sensor's maximum read distance
#define SENSOR_MAX_DISTANCE 200
//  Sensor's read iterations
#define SENSOR_READ_ITERATIONS 5

//  Servo motor's control pin
#define SERVO_CONTROL 8

//  Servo motor's positions
#define SERVO_LEFT_POSITION   2250
#define SERVO_MIDDLE_POSITION 1500
#define SERVO_RIGHT_POSITION  750

//  Defined as centimeters per microsecond to get the distance in centimeters
#define SPEED_OF_SOUND  0.034

//  If an obstacle is closer than this distance (in centimeters), the car will try to evade it
#define OBSTACLE_DISTANCE_THRESHOLD 30

#endif
