#include "car.h"

Car::Car() {
  //  Set motors pin modes
  pinMode(ENABLE_LEFT_MOTOR, OUTPUT);
  pinMode(ENABLE_RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR_IN_1, OUTPUT);
  pinMode(LEFT_MOTOR_IN_2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN_3, OUTPUT);
  pinMode(RIGHT_MOTOR_IN_4, OUTPUT);

  //  Initialize ultra sonic sensor
  sensor = new UltraSonicSensor();

  //  Attach servo control pin to servo and set its position to face forward
  servo.attach(SERVO_CONTROL);
  servo.write(SERVO_MIDDLE_POSITION);
}

void Car::move(CAR_ACTION action, byte speed) {
  //  Turn on both motors and configure them to move forward
  digitalWrite(LEFT_MOTOR_IN_1, LOW);
  digitalWrite(LEFT_MOTOR_IN_2, HIGH);
  digitalWrite(RIGHT_MOTOR_IN_3, LOW);
  digitalWrite(RIGHT_MOTOR_IN_4, HIGH);
  
  switch (action) {
    case FORWARD:
      //  Send PWM signal
      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
      break;
    case BACKWARD:
      //  Turn on both motors and configure them to move backward
      digitalWrite(LEFT_MOTOR_IN_1, HIGH);
      digitalWrite(LEFT_MOTOR_IN_2, LOW);
      digitalWrite(RIGHT_MOTOR_IN_3, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);

      //  Send PWM signal
      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
      break;
    case LEFT:
      //  Send PWM signal
      analogWrite(ENABLE_LEFT_MOTOR, 0);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
      break;
    case RIGHT:
      //  Send PWM signal
      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, 0);
      break;
    case STOP:
      //  Turn off both motors
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, LOW);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);

      //  Send PWM signal
      analogWrite(ENABLE_LEFT_MOTOR, 0);
      analogWrite(ENABLE_RIGHT_MOTOR, 0);
      break;
  }
}

void Car::bypassObstacleOnTheLeftSide() {
  move(BACKWARD, 255);
  delay(1000);
  move(STOP, 0);
  delay(100);
  move(LEFT, 255);
  delay(300);

  int start = millis();
  int current = start;
  while(current - start <= 10000) {
    current = millis();
    
    move(FORWARD, 255);
    delay(500); 
    int right = checkForObstacles(RIGHT_SIDE);
    int front = checkForObstacles(FRONT_SIDE);

    if (front <= OBSTACLE_DISTANCE_THRESHOLD && right <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(BACKWARD, 255);
      delay(500);
      move(LEFT, 255);
      delay(300);
    } else if (front <= OBSTACLE_DISTANCE_THRESHOLD && right > OBSTACLE_DISTANCE_THRESHOLD) {
      move(RIGHT, 255);
      delay(300);
    } else if (front > OBSTACLE_DISTANCE_THRESHOLD && right <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(FORWARD, 255);
      delay(500);
    } else if (front > OBSTACLE_DISTANCE_THRESHOLD && right > OBSTACLE_DISTANCE_THRESHOLD) {
      move(FORWARD, 255);
      delay(500);
      move(RIGHT, 255);
      delay(300);
    }

    if (front <= OBSTACLE_DISTANCE_THRESHOLD && right <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(STOP, 0);
      return;
    }

    if (right > OBSTACLE_DISTANCE_THRESHOLD) {
      delay(500);
      move(RIGHT, 255);
      delay(300);
      move(FORWARD, 255);
    }
  }
}

void Car::bypassObstacleOnTheRightSide() {
  move(BACKWARD, 255);
  delay(1000);
  move(STOP, 0);
  delay(100);
  move(RIGHT, 255);
  delay(300);

  int start = millis();
  int current = start;
  while(current - start <= 10000) {
    current = millis();
    
    move(FORWARD, 255);
    delay(500); 
    int left = checkForObstacles(LEFT_SIDE);
    int front = checkForObstacles(FRONT_SIDE);

    if (front <= OBSTACLE_DISTANCE_THRESHOLD && left <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(BACKWARD, 255);
      delay(500);
      move(LEFT, 255);
      delay(300);
    } else if (front <= OBSTACLE_DISTANCE_THRESHOLD && left > OBSTACLE_DISTANCE_THRESHOLD) {
      move(LEFT, 255);
      delay(300);
    } else if (front > OBSTACLE_DISTANCE_THRESHOLD && left <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(FORWARD, 255);
      delay(500);
    } else if (front > OBSTACLE_DISTANCE_THRESHOLD && left > OBSTACLE_DISTANCE_THRESHOLD) {
      move(FORWARD, 255);
      delay(500);
      move(LEFT, 255);
      delay(300);
    }

    if (front <= OBSTACLE_DISTANCE_THRESHOLD && left <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(STOP, 0);
      return;
    }

    if (left > OBSTACLE_DISTANCE_THRESHOLD) {
      delay(500);
      move(LEFT, 255);
      delay(300);
      move(FORWARD, 255);
    }
  }
}

unsigned int Car::checkForObstacles(DIRECTION direction) {
  switch(direction) {
    case FRONT_SIDE:
      servo.write(SERVO_MIDDLE_POSITION);
      break;
    case RIGHT_SIDE:
      servo.write(SERVO_RIGHT_POSITION);
      break;
    case LEFT_SIDE:
      servo.write(SERVO_LEFT_POSITION);
      break;
  }

  delay(250);
  unsigned int distance = sensor->readSensor();
  delay(250);
  
  return distance;
}

void Car::handleCommand(String command) {
  byte *values = getValuesFromString(command);
  
  byte leftMotorSpeed = values[0];
  byte rightMotorSpeed = values[1];
  byte dir = values[2];
  byte mode = values[3];
  
  //  If the car is in remote control mode, set the driving direction
  if (getControlMode() == REMOTE) {
    if (dir) {  //  Forward
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, HIGH);
    } else {  //  Backward
      digitalWrite(LEFT_MOTOR_IN_1, HIGH);
      digitalWrite(LEFT_MOTOR_IN_2, LOW);
      digitalWrite(RIGHT_MOTOR_IN_3, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);
    }
  }

  //  Set the control mode
  if (mode) {
    setControlMode(AUTONOMOUS);
  } else {
    setControlMode(REMOTE);
  }

  //  Send PWM signal
  analogWrite(ENABLE_LEFT_MOTOR, leftMotorSpeed);
  analogWrite(ENABLE_RIGHT_MOTOR, rightMotorSpeed);

  return values;
}

byte* Car::getValuesFromString(String string) {
  static byte buff[4];
  byte len = string.length();
  byte valueIndex = 0;
  byte beginningIndex = 0;
  
  for (byte i  = 0; i < len; i++) {
    if (string[i] == ' ' || string[i] == '\n') {
      String substr = string.substring(beginningIndex, i);
      buff[valueIndex++] = (byte) substr.toInt();
      beginningIndex = i + 1;
    }
  }

  return buff;
}

void Car::setControlMode(CONTROL_MODE mode) {
  controlMode = mode;
}

CONTROL_MODE Car::getControlMode() {
  return controlMode;
}

void Car::drive() {
  servo.write(SERVO_MIDDLE_POSITION);
  move(FORWARD, 175);
 
  int distance = sensor->readSensor();

  if (distance <= OBSTACLE_DISTANCE_THRESHOLD) {
    move(STOP, 0);

    int left = checkForObstacles(LEFT_SIDE);
    int right = checkForObstacles(RIGHT_SIDE);
    servo.write(SERVO_MIDDLE_POSITION);

    if (left > 50 && right > 50) {
      byte rnd = random(10);

      if (rnd < 5) {
        bypassObstacleOnTheLeftSide();
      } else {
        bypassObstacleOnTheRightSide();
      }
    }

    if (left <= OBSTACLE_DISTANCE_THRESHOLD && right <= OBSTACLE_DISTANCE_THRESHOLD) {
      move(BACKWARD, 255);
      delay(500);

      return;
    }
    
    if (left >= right) {
      bypassObstacleOnTheLeftSide();
    } else {
      bypassObstacleOnTheRightSide();
    }
  }
}
