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
  //  If there's data being sent through serial connection
  if (ESP_serial.available() > 0) {
    //  Read the character
    char character = ESP_serial.read();

    //  Concatenate it to a string
    command += character;

    //  If the character was a new line character,
    //  handle the command and clear the string
    if (character == '\n') {
      car->handleCommands(command);
      command = "";
    }
  }

  //  If the control mode is set to autonomous,
  //  Tell the car to drive itself
  if (car->getControlMode() == AUTONOMOUS) {
    car->drive();
  }
}
