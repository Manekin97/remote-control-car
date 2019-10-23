#include "UltraSonicSensor.h"

UltraSonicSensor::UltraSonicSensor() {
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

unsigned int UltraSonicSensor::readSensor() {
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
