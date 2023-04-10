// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <InputDebounce.h>
#include <AccelStepperWithDistance.h>

/// <summary>
/// This class holds the the stepper motor instance and adds calibration and properties.
/// It uses the external (global) AppSettings instance for initialization.
/// Two limit switches and several inidicator leds are supported.
/// </summary>
class LinearActuator
{
private:
    AccelStepperWithDistance _stepper;          // The stepper instance (AccelStepper).

    InputDebounce _switchStop;                  // Emergency stop switch input.
    InputDebounce _switchLimit1;                // Limit switch 1 (calibration).
    InputDebounce _switchLimit2;                // Limit switch 2 (end of actuator).

    bool _isCalibrating = false;                // Flag indicating that the calibration is running.
    bool _isCalibrated  = false;                // Flag indicating that the calibrationwas successful.
    bool _isStopped     = false;

public : 
    static constexpr const float ACTUATOR_LENGTH = 500.0; // The linear acutator length in mm.

    float getSpeed();                           // Gets the speed in steps per second.
    void  setSpeed(float value);                // Sets the speed in steps per second.
    float getMaxSpeed();                        // Gets the maximum speed in steps per second.
    void  setMaxSpeed(float value);             // Sets the maximum speed in steps per second.
    float getAcceleration();                    // Gets the acceleration in steps per second per second.
    void  setAcceleration(float value);         // Sets the acceleration in steps per second per second.
    long  getSteps();                           // Gets the current position in steps.
    long  getTarget();                          // Gets the target position in steps.
    long  getStepsToGo();                       // Gets the distance to the target positio in steps.
    float getPosition();                        // Gets the current position in mm.

    bool  isEnabled();                          // True if the stepper is enabled.
    bool  isRunning();                          // True if the stepper is running.
    bool  isStopped();                          // True if the stepper has been stopped.
    bool  isCalibrating();                      // True if calibrating.
    bool  isCalibrated();                       // True if calibration was succesful.

    void  enable();                             // Enables the stepper outputs.
    void  disable();                            // Disables the stepper outputs.
    void  home();                               // Move to position zero (home).
    void  reset();                              // Reset the current position to zero.
    void  moveAbsolute(long value);             // Move to absolute position [steps].
    void  moveRelative(long value);             // Move relative distance [steps].
    void  moveAbsoluteDistance(float value);    // Move to absolute position [mm].
    void  moveRelativeDistance(float value);    // Move relative distance [mm] 

    void run();                                 // Check if a step is due and move if necessary.
    void init();                                // Initialize the stepper instance.
    void stop();                                // Stop moving (resetting target position).
    void release();                             // Release stopped motor (clearing flag).

    void switchOn(uint8_t pin);                 // Switch callback routine (on event).
    void switchOff(uint8_t pin);                // Switch callback routine (off event).

    void calibrate();                           // Run the calibration routine.
};