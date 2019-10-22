#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "constants.h"

class UltraSonicSensor {
  public:
    UltraSonicSensor();
    unsigned int readSensor();
};

#endif
