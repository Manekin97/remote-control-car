#include "car.h"

Car::Car() :  algorithm(0),
              controlMode(REMOTE) {
  //  Set motors pin modes
  pinMode(ENABLE_LEFT_MOTOR,  OUTPUT);
  pinMode(ENABLE_RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR_IN_1,    OUTPUT);
  pinMode(LEFT_MOTOR_IN_2,    OUTPUT);
  pinMode(RIGHT_MOTOR_IN_3,   OUTPUT);
  pinMode(RIGHT_MOTOR_IN_4,   OUTPUT);

  //  Initialize ultra sonic sensor
  sensor = new NewPing(TRIGGER, ECHO, 200);

  //  Attach servo control pin to servo and set its position to face forward
  servo.attach(SERVO_CONTROL);
  servo.write(SERVO_MIDDLE_POSITION);
}

void Car::setAlgorithm(const byte algorithmID) {
  algorithm = algorithmID;
}

void Car::simpleAlgorithm() const {
  //  Drive forward
  move(FORWARD, 255);

  //  Get the distance to the obstacle in front
  int distance = sensor->convert_cm(
    sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
  );

  //  If there's an obstacle in front of the vehicle
  if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
    //  Back up
    move(STOP, 0);
    move(BACKWARD, 255);
    delay(500);

    //  Generate a random number in range of <0, 9>
    byte rnd = random(10);

    //  Choose direction based on the random number
    if (rnd < 5) {
      move(LEFT, 255);
    } else {
      move(RIGHT, 255);
    }

    delay(350);
  }
}

int Car::escapeFromCorridor() const {
  Serial.println("escapeFromCorridor");
  int leftDistance = 0, rightDistance = 0;
  
  while (leftDistance <= OBSTACLE_DISTANCE_THRESHOLD && rightDistance <= OBSTACLE_DISTANCE_THRESHOLD) {
    move(BACKWARD, 255);
    delay(250);

    //  Check the distance to an obstacle on the left
    leftDistance = checkForObstacles(LEFT_SIDE);
    //  Check the distance to an obstacle on the right
    rightDistance = checkForObstacles(RIGHT_SIDE);

    delay(250);
  }

  if (leftDistance > OBSTACLE_DISTANCE_THRESHOLD) {
    return LEFT_SIDE;
  } 
  
  if (rightDistance > OBSTACLE_DISTANCE_THRESHOLD) {
    return RIGHT_SIDE;
  }
}
    
void Car::advancedAlgorithm() const {
  //  Drive forward
  move(FORWARD, 255);
  servo.write(SERVO_MIDDLE_POSITION);
  
  //  Get the distance to the obstacle in front
  int distance = checkForObstacles(FRONT_SIDE);

  //  If there's an obstacle in front of the vehicle
  if (distance > 0 && distance < OBSTACLE_DISTANCE_THRESHOLD) {
    move(STOP, 0);
    
    //  Check the distance to an obstacle on the left
    int leftDistance = checkForObstacles(LEFT_SIDE);
    //  Check the distance to an obstacle on the right
    int rightDistance = checkForObstacles(RIGHT_SIDE);

    //  Get the distance to the obstacle in front
    distance = checkForObstacles(FRONT_SIDE);
    
    //  Check if there is enough space to make a turn
    if (distance <= OBSTACLE_DISTANCE_THRESHOLD * 1.5) {
      //  Back up if not
      move(BACKWARD, 255);
      delay(500);
    }

    if (leftDistance <= OBSTACLE_DISTANCE_THRESHOLD && rightDistance <= OBSTACLE_DISTANCE_THRESHOLD) {
      DIRECTION direction = escapeFromCorridor();

      move(BACKWARD, 255);
      delay(500);

      move((int) direction, 255);
    } else if (leftDistance > 0 && leftDistance < rightDistance) {
      move(RIGHT, 255);
    } else if (rightDistance > 0 && leftDistance > rightDistance) {
      move(LEFT, 255);
    } else {
      //  Generate a random number in range of <0, 9>
      byte rnd = random(10);
      
      //  Choose direction based on the random number
      if (rnd < 5) {
        move(LEFT, 255);
      } else {
        move(RIGHT, 255);
      }
    }

    delay(500);
  }
}

void Car::complexAlgorithm() const {
  Serial.println("complexAlgorithm");
}

void Car::move(const CAR_ACTION action, const byte speed) const {
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
      analogWrite(ENABLE_LEFT_MOTOR, 255);
      analogWrite(ENABLE_RIGHT_MOTOR, 255);
      break;
  }
}

void Car::bypassObstacleOnTheLeftSide() const {
  //  Back off the car and make a left turn
  move(BACKWARD, 255);
  delay(350);
  move(STOP, 0);
  delay(100);
  move(LEFT, 255);
  delay(350);

  int start = millis();
  int current = start;
  while (current - start <= 10000) {
    current = millis();

    // Drive forward
    move(FORWARD, 255);
    delay(500);

    //  Check if the car reached the end of the obstacle
    servo.write(SERVO_RIGHT_POSITION);
    delay(100);
    int right = sensor->convert_cm(
      sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
    );

    if (right > 0 && right <= OBSTACLE_DISTANCE_THRESHOLD) {
      //  If the car hasn't reached the end of the obstacle,
      //  check if there's an obstacle in front of the car
      servo.write(SERVO_MIDDLE_POSITION);
      delay(100);
      int front = sensor->convert_cm(
        sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
      );

      if (front > 0 && front <= OBSTACLE_DISTANCE_THRESHOLD) {
        //  If there was, back off and make a left turn
          move(BACKWARD, 255);
          delay(500);
          move(LEFT, 255);
          delay(300);
        
      } else {
        //  Else, keep driving forward
        move(FORWARD, 255);
        delay(500);
      }
    } else {
      /*
       *  If the car reached the end of the obstacle,
       *  check if there's an obstacle in front of the car
       */
     
      servo.write(SERVO_MIDDLE_POSITION);
      delay(100);
      int front = sensor->convert_cm(
        sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
      );

      if (front > 0 && front <= OBSTACLE_DISTANCE_THRESHOLD) {
        //  If there was, make a right turn
        move(RIGHT, 255);
        delay(300);
      } else {
        //  If there wasn't, drive forward a little and make a right turn
        move(FORWARD, 255);
        delay(500);
        move(RIGHT, 255);
        delay(100);
      }

      return; //zmien w right tez
    }
  }
}

//void Car::bypassObstacleOnTheLeftSide() {
//  //  Back off the car and make a left turn
//  move(BACKWARD, 255);
//  delay(350);
//  move(STOP, 0);
//  delay(100);
//  move(LEFT, 255);
//  delay(350);
//
//  int start = millis();
//  int current = start;
//  while (current - start <= 10000) {
//    current = millis();
//
//    // Drive forward
//    move(FORWARD, 255);
//    delay(500);
//
//    //  Check if the car reached the end of the obstacle
//    //  Multiple if statements to prevent false readings from the sensor
//    servo.write(SERVO_RIGHT_POSITION);
//    delay(100);
//    int right = sensor->readSensor();
//
//    if (right <= OBSTACLE_DISTANCE_THRESHOLD) {
//      delay(10);
//      right = sensor->readSensor();
//
//      if (right <= OBSTACLE_DISTANCE_THRESHOLD) {
//        //  If the car hasn't reached the end of the obstacle,
//        //  check if there's an obstacle in front of the car
//        servo.write(SERVO_MIDDLE_POSITION);
//        delay(100);
//        int front = sensor->readSensor();
//
//        if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//          delay(10);
//          front = sensor->readSensor();
//
//          //  If there was, back off and make a left turn
//          if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//            move(BACKWARD, 255);
//            delay(500);
//            move(LEFT, 255);
//            delay(300);
//          }
//        } else {
//          //  Else, keep driving forward
//          move(FORWARD, 255);
//          delay(500);
//        }
//      }
//    } else {
//      //  If the car reached the end of the obstacle,
//      //  check if there's an obstacle in front of the car
//      servo.write(SERVO_MIDDLE_POSITION);
//      delay(100);
//      int front = sensor->readSensor();
//
//      if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//        delay(10);
//        front = sensor->readSensor();
//
//        if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//          //  If there was, make a right turn
//          move(RIGHT, 255);
//          delay(300);
//        }
//      } else {
//        //  If there wasn't, drive forward a little and make a right turn
//        move(FORWARD, 255);
//        delay(500);
//        move(RIGHT, 255);
//        delay(100);
//
//        return;
//      }
//    }
//  }
//}

void Car::bypassObstacleOnTheRightSide() const {
  //  Back off the car and make a right turn
  move(BACKWARD, 255);
  delay(350);
  move(STOP, 0);
  delay(100);
  move(RIGHT, 255);
  delay(350);

  int start = millis();
  int current = start;
  while (current - start <= 10000) {
    current = millis();

    // Drive forward
    move(FORWARD, 255);
    delay(500);

    //  Check if the car reached the end of the obstacle
    servo.write(SERVO_LEFT_POSITION);
    delay(100);
    int left = sensor->convert_cm(
      sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
    );

    if (left > 0 && left <= OBSTACLE_DISTANCE_THRESHOLD) {
      //  If the car hasn't reached the end of the obstacle,
      //  check if there's an obstacle in front of the car
      servo.write(SERVO_MIDDLE_POSITION);
      delay(100);
      int front = sensor->convert_cm(
        sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
      );

      if (front > 0 && front <= OBSTACLE_DISTANCE_THRESHOLD) {
          move(BACKWARD, 255);
          delay(500);
          move(RIGHT, 255);
          delay(300);
      } else {
        //  Else, keep driving forward
        move(FORWARD, 255);
        delay(500);
      }
    }  else {
      /*
       *  If the car reached the end of the obstacle,
       *  check if there's an obstacle in front of the car
       */

      servo.write(SERVO_MIDDLE_POSITION);
      delay(100);
      int front = sensor->convert_cm(
        sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
      );
  
      if (front > 0 && front <= OBSTACLE_DISTANCE_THRESHOLD) {
        //  If there was, make a left turn
        move(LEFT, 255);
        delay(300);
      } else {
        //  If there wasn't, drive forward a little and make a left turn
        move(FORWARD, 255);
        delay(500);
        move(LEFT, 255);
        delay(100);
  
        return;
      }
    }
  }
}

//void Car::bypassObstacleOnTheRightSide() {
//  //  Back off the car and make a right turn
//  move(BACKWARD, 255);
//  delay(350);
//  move(STOP, 0);
//  delay(100);
//  move(RIGHT, 255);
//  delay(350);
//
//  int start = millis();
//  int current = start;
//  while (current - start <= 10000) {
//    current = millis();
//
//    // Drive forward
//    move(FORWARD, 255);
//    delay(500);
//
//    //  Check if the car reached the end of the obstacle
//    //  Multiple if statements to prevent false readings from the sensor
//    servo.write(SERVO_LEFT_POSITION);
//    delay(100);
//    int left = sensor->readSensor();
//
//    if (left <= OBSTACLE_DISTANCE_THRESHOLD) {
//      delay(10);
//      left = sensor->readSensor();
//
//      if (left <= OBSTACLE_DISTANCE_THRESHOLD) {
//        //  If the car hasn't reached the end of the obstacle,
//        //  check if there's an obstacle in front of the car
//        servo.write(SERVO_MIDDLE_POSITION);
//        delay(100);
//        int front = sensor->readSensor();
//
//        if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//          delay(10);
//          front = sensor->readSensor();
//
//          //  If there was, back off and make a right turn
//          if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//            move(BACKWARD, 255);
//            delay(500);
//            move(RIGHT, 255);
//            delay(300);
//          }
//        } else {
//          //  Else, keep driving forward
//          move(FORWARD, 255);
//          delay(500);
//        }
//      }
//    } else {
//      //  If the car reached the end of the obstacle,
//      //  check if there's an obstacle in front of the car
//      servo.write(SERVO_MIDDLE_POSITION);
//      delay(100);
//      int front = sensor->readSensor();
//
//      if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//        delay(10);
//        front = sensor->readSensor();
//
//        if (front <= OBSTACLE_DISTANCE_THRESHOLD) {
//          //  If there was, make a left turn
//          move(LEFT, 255);
//          delay(300);
//        }
//      } else {
//        //  If there wasn't, drive forward a little and make a left turn
//        move(FORWARD, 255);
//        delay(500);
//        move(LEFT, 255);
//        delay(100);
//
//        return;
//      }
//    }
//  }
//}

unsigned int Car::checkForObstacles(const DIRECTION direction) const {
  //  Position servo at the correct position
  switch (direction) {
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

  //  Wait 100ms so that the servo has time to rotate
  delay(100);

  //  Read the sensor
  unsigned int distance = sensor->convert_cm(
    sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
  );

  //  Return the distance
  return distance;
}

void Car::handleCommand(const String command) {
  //  Get the values from the command string
  byte *values = getValuesFromString(command);

  //  Assign them to variables
  byte leftMotorSpeed = values[0];
  byte rightMotorSpeed = values[1];
  byte dir = values[2];
  byte mode = values[3];
  byte algorithm = values[4];

  //  If the car is in remote control mode, set the driving direction
  if (getControlMode() == REMOTE) {
    if (dir) {  
      //  Forward
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, HIGH);
    } else {  
      //  Backward
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

  //  Set the AI algorithm  
  setAlgorithm(algorithm);

  //  Send PWM signal
  analogWrite(ENABLE_LEFT_MOTOR, leftMotorSpeed);
  analogWrite(ENABLE_RIGHT_MOTOR, rightMotorSpeed);
}

byte* Car::getValuesFromString(const String string) const {
  //  Create a static buffer
  static byte buff[5];
  
  //  Get the length of the string
  byte len = string.length();
  byte valueIndex = 0;
  byte beginningIndex = 0;

  //  Iterate over each character
  for (byte i  = 0; i < len; i++) {
    //  If the characters is a space
    if (string[i] == ' ') {
      //  Get a substring from the last space position + 1, to the (i - 1)-th position
      String substr = string.substring(beginningIndex, i);
      //  Parse the substring to an integer, cast it to byte and add the value to the buffer
      buff[valueIndex++] = (byte) substr.toInt();
      //  Set the beginning index to the (i + 1)-th position
      beginningIndex = i + 1;
    }
  }

  //  Return a pointer to the buffer
  return buff;
}

void Car::setControlMode(const CONTROL_MODE mode) {
  /*
   *  The ServoTimer2 library uses the same timer as SoftwareSerial library,
   *  which causes the servo to jitter when receveing data.
   *  To prevent that set the pin mode to INPUT_PULLUP
   *  when the car is in remote control mode.
   */
   
  if (mode == REMOTE) {
    pinMode(SERVO_CONTROL, INPUT_PULLUP);
  } else {
    pinMode(SERVO_CONTROL, OUTPUT);
  }
  
  controlMode = mode;
}

CONTROL_MODE Car::getControlMode() const {
  return controlMode;
}

void Car::drive() {
  switch(algorithm) {
    case SIMPLE:
      simpleAlgorithm();
      break;
    case ADVANCED:
      advancedAlgorithm();
      break;
    case COMPLEX:
      complexAlgorithm();
      break;
    default:
      break;
  }
//  //  Drive forward
//  servo.write(SERVO_MIDDLE_POSITION);
//  move(FORWARD, 175);
//
//  int distance = sensor->convert_cm(
//    sensor->ping_median(SENSOR_READ_ITERATIONS, SENSOR_MAX_DISTANCE)
//  );
//
//
//  if (distance > 0 && distance <= OBSTACLE_DISTANCE_THRESHOLD) {
//    move(STOP, 0);
//    delay(100);
//    Serial.println("przeszkoda - front");
//
//    //  Check the left side
//    int left = checkForObstacles(LEFT_SIDE);
//
//    //  Check the right side
//    int right = checkForObstacles(RIGHT_SIDE);
//
//    servo.write(SERVO_MIDDLE_POSITION);
//    Serial.print("left=");
//    Serial.println(left);
//    Serial.print("right=");
//    Serial.println(right);
//
//    //  If there are no obstacles on both sides
//    if (left == 0 && right == 0) {
//      Serial.println("left == 0 && right == 0");
////      //  Choose a random side
////      byte rnd = random(10);
////
////      if (rnd < 5) {
////        bypassObstacleOnTheLeftSide();
////      } else {
////        bypassObstacleOnTheRightSide();
////      }
//    } else if ((left > 0 && left <= OBSTACLE_DISTANCE_THRESHOLD) && (right > 0 && right <= OBSTACLE_DISTANCE_THRESHOLD)) {
//      Serial.println("(left > 0 && left <= OBSTACLE_DISTANCE_THRESHOLD) && (right > 0 && right <= OBSTACLE_DISTANCE_THRESHOLD");
//      //  If there are obstacles on both sides, back off the car
////      move(BACKWARD, 255);
////      delay(500);
//    } else if ((left >= right && right > 0) || left == 0) {
//      //  If the obstacle on the right side is closer than on the left side
//      bypassObstacleOnTheLeftSide();
//    } else {
//      Serial.println("else");
//      //  Else, if the obstacle on the left is closer than on the right side
////      bypassObstacleOnTheRightSide();
//    }
//  }
}

//void Car::drive() {
//  //  Drive forward
//  servo.write(SERVO_MIDDLE_POSITION);
//  move(FORWARD, 175);
//
//  int distance = sensor->readSensor();
//
//  //  Multiple if statements to prevent false readings from the sensor
//  if (distance <= OBSTACLE_DISTANCE_THRESHOLD) {
//    delay(10); // błaadd
//    if (distance <= OBSTACLE_DISTANCE_THRESHOLD) {
//      move(STOP, 0);
//
//      //  Check the left side (Twice to avoid false readings)
//      int left = checkForObstacles(LEFT_SIDE);
//      delay(10);
//      left = checkForObstacles(LEFT_SIDE);
//
//      //  Check the right side (Twice to avoid false readings)
//      int right = checkForObstacles(RIGHT_SIDE);
//      delay(10);
//      right = checkForObstacles(RIGHT_SIDE);
//
//      servo.write(SERVO_MIDDLE_POSITION);
//
//      //  If there are no obstacles on both sides
//      if (left > 50 && right > 50) {
//        //  Choose a random side
//        byte rnd = random(10);
//
//        if (rnd < 5) {
//          bypassObstacleOnTheLeftSide();
//        } else {
//          bypassObstacleOnTheRightSide();
//        }
//      }  else if (left <= OBSTACLE_DISTANCE_THRESHOLD && right <= OBSTACLE_DISTANCE_THRESHOLD ) {
//        //  If there are obstacles on both sides, back off the car
//        move(BACKWARD, 255);
//        delay(500);
//      } else if (left >= right) {
//        //  If the obstacle on the right side is closer than on the left side
//        bypassObstacleOnTheLeftSide();
//      } else {
//        //  Else, if the obstacle on the left is closer than on the right side
//        bypassObstacleOnTheRightSide();
//      }
//    }
//  }
//}
