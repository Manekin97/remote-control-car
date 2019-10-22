#include <SoftwareSerial.h>
#include "Car.h"

//  Used to store command string
String command;

//  Software serial communication used for communication with ESP8266 board
SoftwareSerial ESP_serial(2, 3);  //  Tx, Rx

//  The car object
Car *car;

void setup() {
  //  Setup serial connection
  ESP_serial.begin(9600);
  Serial.begin(115200);

  //  Create the car
  car = new Car();
}

void loop() {
  if (ESP_serial.available() > 0) {
    char character = ESP_serial.read();

    command += character;
    if (character == '\n') {
      car->handleCommands(command);
      command = "";
    }
  }

  if (car->getControlMode() == AUTONOMOUS) {
    car->drive();
  }
}
