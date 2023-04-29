// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>29-4-2023 5:46 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Arduino.h>

#include "TimerInterrupt_Generic.h"

extern bool TimerHandler(struct repeating_timer* t);

/// <summary>
/// This class holds the the stepper motor instance and adds calibration and properties.
/// It uses the external (global) AppSettings instance for initialization.
/// Two limit switches and several inidicator leds are supported.
/// 
/// A typical NEMA stepper motor has the following characteristics:
/// 
///     Step Angle : 1.8°(resulting in 200 steps per rotation).
///     Max.PRM : 600  (resulting in 2000 steps per second or 2kHz).
/// 
/// With higher frequency the usable torque is decreased considerably.
/// 
/// A typical stepper motor driver such as the DM542T provides several control ports.
/// 
///     PUL+, PUL-:     Pulsesignal: In single pulse (pulse/direction) mode, this input 
///                     represents pulse signal, each rising edge active;
/// 
///     DIR+, DIR-:     In single-pulse mode, this signal has low/high voltage levels, 
///                     representing two directions of motor rotation. The motor rotation 
///                     direction is also related to motor - drive wiring match.
///                     Exchanging the connection of two wires for a coil to the drive will
///                     reverse motor direction.
/// 
/// Pulse, Direction and Enable Connection:
/// 
///     - Optically isolated, high level 4.5 - 5V or 24V, low voltage 0 - 0.5V.
///     - The width of PUL signal is at least 2.5μs, duty cycle is recommended 50 %.
///     - DIR signal requires advance PUL signal minimum 5 μs in single pulse mode.
///     - ENA must be ahead of DIR by at least 200ms. Usually, ENA+ and ENA- are NC (not connected).
/// 
/// As a stepper motor driver allows microstepping (typically 16 microsteps) this leads to a frequency
/// of 32 kHz for the maximum of 600 RPM (16 * 200 * 600 / 60).
/// A timer is used to call a routine every 10 microseconds (100 kHz). An internal counter is used to
/// keep track of pulse start, pulse end, and the delay between every pulse. The interval setting is 
/// the number the timer callback routine is called before a new pulse is generated.
/// </summary>
class LinearActuator
{
public:
    enum Direction
    {
        CW  = 1,
        CCW = -1
    };

private:
    uint8_t _PUL;                               // GPIO pin number for the pulse (PUL+) pin.
    uint8_t _DIR;                               // GPIO pin number for the direction (DIR+) pin.
    uint8_t _ENA;                               // GPIO pin number for the enable (ENA+) pin.

    uint8_t _ledRunning;                        // GPIO pin number for the running LED.
    uint8_t _ledInLimit;                        // GPIO pin number for the limit LED.
    uint8_t _ledAlarmOn;                        // GPIO pin number for the alarm LED.

    bool _calibratingFlag = false;              // Flag indicating that the calibration is running.
    bool _calibratedFlag  = false;              // Flag indicating that the calibration was successful.
    bool _enabledFlag = false;                  // Flag indicating that the stepper has been enabled.
    bool _limitFlag = false;                    // Flag indicating that the stepper has hit a limit switch.
    bool _alarmFlag = false;                    // Flag indicating that the stepper alarm singal has been turned on.

    Direction _direction = Direction::CW;       // Stepper driver direction (CW: 1, CCW: -1).
    float     _distancePerRotation = 2;         // Distance per full rotation (mm).
    ushort    _stepsPerRotation = 200;          // Steps per full rotation (360°).
    ushort    _microsteps = 1;                  // Stepper driver microstep settings.
    uint      _interval = 0;                    // Current timer interval (microseconds).
    long      _position = 0;                    // Current stepper position (steps).
    long      _target = 0;                      // Target position (steps).
    uint32_t  _count = 0;                       // Timer count.

    void _ccw();                                // Turn off the direction pin.
    void _cw();                                 // Turn on the direction pin.

public : 
    uint      getCount();
    uint      getInterval();                    // Gets the timer interval (delay between pulses) in microseconds.
    void      setInterval(uint value);          // Sets the timer interval (delay between pulses) in microseconds.
    ushort    getMicroSteps();                  // Gets the microsteps for the stepper driver.
    void      setMicroSteps(ushort value);      // Sets the microsteps for the stepper driver.
    long      getSteps();                       // Gets the current position in steps.
    long      getTarget();                      // Gets the target position in steps.
    long      getStepsToGo();                   // Gets the distance to the target position in steps.
    float     getPosition();                    // Gets the current position in mm.
    Direction getDirection();                   // Gets the current direction.

    bool  getEnabledFlag();                     // True if the stepper is enabled.
    bool  getRunningFlag();                     // True if the stepper is running (position != target).
    bool  getLimitFlag();                       // True if the stepper has hit a limit switch.
    bool  getAlarmFlag();                       // True if the stepper alarm signal is on.
    bool  getCalibratingFlag();                 // True if calibrating.
    bool  getCalibratedFlag();                  // True if calibration was succesful.

    void  enable();                             // Enables the stepper outputs.
    void  disable();                            // Disables the stepper outputs.
    void  home();                               // Move to position zero (home).
    void  reset();                              // Reset the current position to zero.
    void  retract(long value);                  // Retract a short distance.
    void  moveAbsolute(long value);             // Move to absolute position [steps].
    void  moveRelative(long value);             // Move relative distance [steps].
    void  moveAbsoluteDistance(float value);    // Move to absolute position [mm].
    void  moveRelativeDistance(float value);    // Move relative distance [mm].

    void init();                                // Initialize the stepper instance.
    void stop();                                // Stop moving (resetting target position, disable output).
    void release();                             // Release stopped motor (clearing flags, enable output).

    void alarmOn(uint8_t pin);                  // Alarm callback routine (on event).
    void alarmOff(uint8_t pin);                 // Alarm callback routine (off event).

    void switchOn(uint8_t pin);                 // Switch callback routine (on event).
    void switchOff(uint8_t pin);                // Switch callback routine (off event).

    void calibrate();                           // Run the calibration routine.
    void onTimer();                             // Timer callback routine.
};