// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>25-4-2023 7:55 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Arduino.h>
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

    bool _constantSpeed = false;                // Flag indicating that no speed calculation is done.
    bool _getCalibratingFlag = false;                // Flag indicating that the calibration is running.
    bool _getCalibratedFlag  = false;                // Flag indicating that the calibration was successful.
    bool _getLimitFlag = false;                    // Flag indicating that the stepper has hit a limit switch.
    bool _getAlarmFlag = false;                    // Flag indicating that the stepper alarm singal has been turned on.

    ushort _microsteps = 1;                     // Stepper driver microstep settings.
    ushort _pulsewidth = 10;                    // Stepper driver minimum pulse width.

public : 
    static constexpr const float ACTUATOR_LENGTH = 500.0; // The linear acutator length in mm.

    float  getSpeed();                          // Gets the speed in steps per second.
    void   setSpeed(float value);               // Sets the speed in steps per second.
    float  getMaxSpeed();                       // Gets the maximum speed in steps per second.
    void   setMaxSpeed(float value);            // Sets the maximum speed in steps per second.
    float  getAcceleration();                   // Gets the acceleration in steps per second per second.
    void   setAcceleration(float value);        // Sets the acceleration in steps per second per second.
    ushort getPulseWidth();                     // Gets the minimum pulsewidth in microseconds.
    void   setPulseWidth(ushort value);         // Sets the minimum pulsewidth in microseconds.
    ushort getMicroSteps();                     // Gets the microsteps for the stepper driver.
    void   setMicroSteps(ushort value);         // Sets the microsteps for the stepper driver.
    long   getSteps();                          // Gets the current position in steps.
    long   getTarget();                         // Gets the target position in steps.
    long   getStepsToGo();                      // Gets the distance to the target positio in steps.
    float  getPosition();                       // Gets the current position in mm.

    bool  getEnabledFlag();                     // True if the stepper is enabled.
    bool  getRunningFlag();                     // True if the stepper is running.
    bool  getLimitFlag();                       // True if the stepper has hit a limit switch.
    bool  getAlarmFlag();                       // True if the stepper alarm signal is on.
    bool  getCalibratingFlag();                 // True if calibrating.
    bool  getCalibratedFlag();                  // True if calibration was succesful.
    bool  getConstantSpeedFlag();               // True if running with constant speed.

    void  enable();                             // Enables the stepper outputs.
    void  disable();                            // Disables the stepper outputs.
    void  home();                               // Move to position zero (home).
    void  reset();                              // Reset the current position to zero.
    void  retract(long value);                  // Retract a short distance.
    void  moveAbsolute(long value);             // Move to absolute position [steps].
    void  moveRelative(long value);             // Move relative distance [steps].
    void  moveAbsoluteDistance(float value);    // Move to absolute position [mm].
    void  moveRelativeDistance(float value);    // Move relative distance [mm].

    void run();                                 // Check if a step is due and move if necessary.
    void init();                                // Initialize the stepper instance.
    void stop();                                // Stop moving (resetting target position).
    void release();                             // Release stopped motor (clearing flag).

    void alarmOn(uint8_t pin);                  // Alarm callback routine (on event).
    void alarmOff(uint8_t pin);                 // Alarm callback routine (off event).

    void switchOn(uint8_t pin);                 // Switch callback routine (on event).
    void switchOff(uint8_t pin);                // Switch callback routine (off event).

    void calibrate();                           // Run the calibration routine.
};