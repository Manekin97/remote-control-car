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
  LEFT_SIDE,
  NORTH_EAST,
  NORTH_WEST
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

//  Software serial pins
#define SOFTWARE_SERIAL_TX  2
#define SOFTWARE_SERIAL_RX  3

//  Motor control pins
#define ENABLE_LEFT_MOTOR   9
#define ENABLE_RIGHT_MOTOR  10
#define LEFT_MOTOR_IN_1     4
#define LEFT_MOTOR_IN_2     5
#define RIGHT_MOTOR_IN_3    6
#define RIGHT_MOTOR_IN_4    7

//  Sensor pins
#define ECHO    12
#define TRIGGER 13

//  Sensor maximum read distance
#define SENSOR_MAX_DISTANCE 200
//  Sensor read iterations
#define SENSOR_READ_ITERATIONS 5

//  Servo motor control pin
#define SERVO_CONTROL 8

//  Servo motor positions
#define SERVO_LEFT_POSITION       2250
#define SERVO_NORTH_EAST_POSITION 1875
#define SERVO_MIDDLE_POSITION     1500
#define SERVO_NORTH_WEST_POSITION 1125
#define SERVO_RIGHT_POSITION      750

//  Defined as centimeters per microsecond to get the distance in centimeters
#define SPEED_OF_SOUND  0.034

//  If an obstacle is closer than this distance (in centimeters), the car will try to evade it
#define OBSTACLE_DISTANCE_THRESHOLD 40


#endif
