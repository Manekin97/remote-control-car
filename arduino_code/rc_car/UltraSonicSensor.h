#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "constants.h"

class UltraSonicSensor {
  public:
    //  The constructor, it initializes pin modes
    UltraSonicSensor();

    //  Read the distance and return it in centimeters
    unsigned int readSensor();
};

#endif
