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

unsigned int distance;

//  Servo motor pins
#define SERVO_CONTROL 8

Servo servo;
int servoPosition = 90;
int change = 1;

//  Defined as centimeters per microsecond to get the distance in centimeters
const float SPEED_OF_SOUND = 0.034;

int payload = 5;

SoftwareSerial ESP_serial(2, 3);  //  Tx, Rx
enum CAR_ACTION {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  STOP
};

unsigned long timeOfStop = 0;

void setup() {
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
  servo.attach(SERVO_CONTROL);

  //  Setup timer interrupt
  
   noInterrupts();
   
  // Set compare match register for 100Hz increments
  OCR2A = 156;
  // turn on CTC mode
  TCCR2B |= (1 << WGM12);
  // Set prescaler to 1024
  TCCR2B |= (1 << CS12) | (1 << CS10);
  // Enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  interrupts();
}

ISR(TIMER2_COMPA_vect){
  distance = readSensor();
}

void loop() {
  scanForObstacles();
  
  if (ESP_serial.available() > 0) {
    char payload_raw = ESP_serial.read();
    payload = (int)payload_raw - 48;
    timeOfStop = millis();
  }

  switch (payload) {
    case 0:
      move(BACKWARD, 215);
      break;
    case 1:
      move(RIGHT, 215);
      break;
    case 2:
      move(LEFT, 215);
      break;
    case 3:
      move(FORWARD, 215);
      break;
    default:
      if (millis() - timeOfStop > 100){
          move(STOP, 215);
      }
  }
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

      analogWrite(ENABLE_LEFT_MOTOR, speed);
      analogWrite(ENABLE_RIGHT_MOTOR, speed * 0.75);
      break;
    case RIGHT:
      digitalWrite(LEFT_MOTOR_IN_1, LOW);
      digitalWrite(LEFT_MOTOR_IN_2, HIGH);
      digitalWrite(RIGHT_MOTOR_IN_3, LOW);
      digitalWrite(RIGHT_MOTOR_IN_4, LOW);

      analogWrite(ENABLE_LEFT_MOTOR, speed * 0.75);
      analogWrite(ENABLE_RIGHT_MOTOR, speed);
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

  return dist;
}
