#ifndef CAR_H
#define CAR_H

#include <ServoTimer2.h>
#include <NewPing.h>
#include "UltraSonicSensor.h"
#include "constants.h"
#include <Arduino.h>

/**
 * Implementation of the vehicle functionality
 */
class Car {

  //  Class fields
  public:
    //  Global variable used for storing the control mode
    CONTROL_MODE controlMode;

    //  Ultra sonic sensor object
    NewPing *sensor;

    //  Servo control object
    ServoTimer2 servo;

    //  AI Algorithm
    AI_ALGORITHM algorithm;

  //  Class methods
  public:

    /***
    * Initialize pin modes, create sensor object 
    * and attache servo to its control pin
    *
    * @returns a pointer to the car object.
    */
    Car();
    
    //  This function tells the car how to drive
    void drive();

    /***
    * Get the vehicle's control mode.
    *
    * @returns Current control mode of the vehicle
    */
    CONTROL_MODE getControlMode() const;

    /***
    * Execute the command.
    *
    * @param[in] command - the command to be handled
    * @returns void
    */
    void handleCommand(const String command);
    
  private:

    /**
    * Set the vehicle's AI algorithm.
    *
    * @param[in] algorithmID - the id of set algorithm
    * @returns void
    */
    void setAlgorithm(const byte algorithmID);

    /**
    * Set the vehicle's control mode.
    *
    * @param[in] mode - the set control mode
    * @returns void
    */
    void setControlMode(const CONTROL_MODE mode);

    /**
    * The simplest AI algorithm
    *
    * @returns void
    */
    void simpleAlgorithm() const;

    /**
    * The advanced AI algorithm
    *
    * @returns void
    */
    void advancedAlgorithm() const;

    /**
    * The complex AI algorithm
    *
    * @returns void
    */
    void complexAlgorithm() const;

    /**
    * Escape a corridor (when there are obstacles 
    * in front and on both sides of the vehicle)
    *
    * @returns the direction in which the vehicles should turn 
    * after escaping the corridor
    */
    int escapeFromCorridor() const;
    
    /**
    * Move the vehicle in chosen direction with chosen speed
    *
    * @param[in] action - the action the vehicle should perform
    * @param[in] speed  - the speed of the vehicle
    * @returns void
    */
    void move(const CAR_ACTION action, const byte speed) const;

    /**
    * Parse the command string
    *
    * @param[in] string - the string containing instruction values
    * @returns an array of instruction values
    */
    byte* getValuesFromString(const String string) const;

    /**
    * Bypass the obstacle on the left side
    *
    * @returns void
    */
    void bypassObstacleOnTheLeftSide() const;

    /**
    * Bypass the obstacle on the right side
    *
    * @returns void
    */
    void bypassObstacleOnTheRightSide() const;

    /**
    * Check for obstacle in the desired direction
    *
    * @param[in] direction - the direction to check
    * @returns the distance to the obstacle
    */
    unsigned int checkForObstacles(const DIRECTION direction) const;
};

#endif
