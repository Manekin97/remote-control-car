#include <SoftwareSerial.h>
#include <Servo.h>

//  Motor control pins
#define ENABLE_LEFT_MOTOR   9
#define ENABLE_RIGHT_MOTOR  10
#define LEFT_MOTOR_IN_1     4
#define LEFT_MOTOR_IN_2     5
#define RIGHT_MOTOR_IN_3    6
#define RIGHT_MOTOR_IN_4    7

// Sensor pins
#define ECHO    12
#define TRIGGER 13

//  Servo motor pins
#define SERVO_CONTROL 8

unsigned int distance;

Servo servo;
int servoPosition = 90;
int change = 1;

//  Defined as centimeters per microsecond to get the distance in centimeters
const float SPEED_OF_SOUND = 0.034;

//  If an obstacle is closer than this distance (in centimeters), the car will try to evade it
const float OBSTACLE_DISTANCE_THRESHOLD= 30;

String command;
int values[4];
unsigned long timeOfStop = 0;

SoftwareSerial ESP_serial(2, 3);  //  Tx, Rx

enum CAR_ACTION {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  STOP
};

enum CONTROL_MODE {
  REMOTE,
  AUTONOMOUS
};

CONTROL_MODE controlMode = REMOTE;

void setup() {
  //  Setup serial connection
  ESP_serial.begin(9600);
  Serial.begin(115200);

  //  Motors
  pinMode(ENABLE_LEFT_MOTOR, OUTPUT);
  pinMode(ENABLE_RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR_IN_1, OUTPUT);
  pinMode(LEFT_MOTOR_IN_2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN_3, OUTPUT);
  pinMode(RIGHT_MOTOR_IN_4, OUTPUT);

  //  Sensor
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  //  Attach servo control pin to servo
//  servo.attach(SERVO_CONTROL);
}

void loop() {
  if (ESP_serial.available() > 0) {
    char character = ESP_serial.read();
    
    command += character;
    if (character == '\n') {
      getValuesFromString(command, values);
      command = "";
      
      handleCommands(values[0], values[1], values[2], values[3]);
    }

    timeOfStop = millis();
  }

  if (controlMode == AUTONOMOUS) {
    move(FORWARD, 255);
    servo.write(90);
    distance = readSensor();

    if(distance < OBSTACLE_DISTANCE_THRESHOLD) {
      servo.write(180);
      distance = readSensor();

      if (distance < OBSTACLE_DISTANCE_THRESHOLD) {
        servo.write(0);
        distance = readSensor();
        
        if(distance < OBSTACLE_DISTANCE_THRESHOLD) {
          move(BACKWARD, 255);
          delay(1000);
        } else {
          move(RIGHT, 255);
          delay(300);
        }
      } else {
        move(LEFT, 255);
        delay(300);
      }
    }
  }
}

void handleCommands(int left, int right, int dir, int mode) {
  if (dir) {
    digitalWrite(LEFT_MOTOR_IN_1, LOW);
    digitalWrite(LEFT_MOTOR_IN_2, HIGH);
    digitalWrite(RIGHT_MOTOR_IN_3, LOW);
    digitalWrite(RIGHT_MOTOR_IN_4, HIGH);
  } else {
    digitalWrite(LEFT_MOTOR_IN_1, HIGH);
    digitalWrite(LEFT_MOTOR_IN_2, LOW);
    digitalWrite(RIGHT_MOTOR_IN_3, HIGH);
    digitalWrite(RIGHT_MOTOR_IN_4, LOW);
  }

  if (mode) {
    setMode(AUTONOMOUS);
  } else {
    setMode(REMOTE);
  }

  analogWrite(ENABLE_LEFT_MOTOR, left);
  analogWrite(ENABLE_RIGHT_MOTOR, right);
}

void getValuesFromString(String string, int *buff) {
  int len = string.length();
  int valueIndex = 0;
  int beginningIndex = 0;
  
  for (int i  = 0; i < len; i++) {
    if (string[i] == ' ') {
      String substr = string.substring(beginningIndex, i);
      buff[valueIndex++] = substr.toInt();
      beginningIndex = i + 1;
    }
  }
}

void handleCommand(int commandCode) {
  if(controlMode == REMOTE) {
    switch (commandCode) {
      case 0:
        move(BACKWARD, 255);
        break;
      case 1:
        move(RIGHT, 255);
        break;
      case 2:
        move(LEFT, 255);
        break;
      case 3:
        move(FORWARD, 255);
        break;
      default:
        if (millis() - timeOfStop > 100){
            move(STOP, 215);
        }
      }
  }
}

void setControlMode(CONTROL_MODE mode) {
  controlMode = mode;
}

void scanForObstacles() {
  if(servoPosition <= 0 || servoPosition >= 180) {
    change = -change;
  }

  servoPosition += change;
  servo.write(servoPosition);
  
  delay(10);
}

void move(CAR_ACTION action, int speed) {
  switch (action) {
    case FORWARD:
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, HIGH);

      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
      break;
    case BACKWARD:
      digitalWrite(LEFT_MOTOR_IN_1, HIGH);
      digitalWrite(LEFT_MOTOR_IN_2, LOW);
      digitalWrite(RIGHT_MOTOR_IN_3, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);

      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
      break;
    case LEFT:
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, LOW);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, HIGH);

      analogWrite(ENABLE_LEFT_MOTOR, 0);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
      break;
    case RIGHT:
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);

      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, 0);
      break;
    case STOP:
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, LOW);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);

      analogWrite(ENABLE_LEFT_MOTOR, 0);
      analogWrite(ENABLE_RIGHT_MOTOR, 0);
      break;
  }
}

unsigned int readSensor() {
  //  Clear trigger pin
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  
  // Set the trigger pin on HIGH state for 10 microseconds
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  
  // Read the echo pin
  unsigned long duration = pulseIn(ECHO, HIGH);
  
  // Calculate the distance
  unsigned int dist = duration * SPEED_OF_SOUND / 2;

  Serial.println(dist);
  return dist;
}
