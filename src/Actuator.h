// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>19-5-2023 9:21 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Arduino.h>

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
///     - The width of PUL signal is at least 2.5µs, duty cycle is recommended 50 %.
///     - DIR signal requires advance PUL signal minimum 5 µs in single pulse mode.
///     - ENA must be ahead of DIR by at least 200ms. Usually, ENA+ and ENA- are NC (not connected).
/// 
/// As a stepper motor driver allows microstepping (typically 16 microsteps) this leads to a frequency
/// of 32 kHz for the maximum of 600 RPM (16 * 200 * 600 / 60).
/// A timer is used to call a routine every 10 microseconds (100 kHz). An internal counter is used to
/// keep track of pulse start, pulse end, and the delay between every pulse. The resulting interval is 
/// the number the timer callback routine is called before a new pulse is generated.
/// The starting interval is determined by the minimum speed and decreased to reach the maximum speed.
/// Before reaching the target, the interval is increased again until the minimum speed is reached.
/// </summary>
class LinearActuator
{
public:
    static constexpr const uint  FREQUENCY = 100000;              // The timer frequency 100 kHz (pulsewidth = 10 usec).
    static constexpr const uint  INTERVAL  = 1000000 / FREQUENCY; // The time between callbacks (microseconds).
    static constexpr const float MIN_SPEED = 1;                   // The minimum speed (1 step per second).
    static constexpr const float MAX_SPEED = FREQUENCY / 2.0;     // The maximum speed (50000 steps per second).
    static constexpr const float DIR_DELAY  = 200;                // The delay (ms) for direction change.

    enum Direction
    {
        CW  = 1,                                    // Clockwise direction.
        CCW = -1                                    // Counter Clockwise direction.
    };

private:
    StaticJsonDocument<384> _doc;                   // The Json document representing the status data.

    uint8_t _PUL;                                   // GPIO pin number for the pulse (PUL+) pin.
    uint8_t _DIR;                                   // GPIO pin number for the direction (DIR+) pin.
    uint8_t _ENA;                                   // GPIO pin number for the enable (ENA+) pin.
    uint8_t _ALM;                                   // GPIO pin number for the alarm (ALM+) pin.

    uint8_t _ledRunning;                            // GPIO pin number for the running LED.
    uint8_t _ledInLimit;                            // GPIO pin number for the limit LED.
    uint8_t _ledAlarmOn;                            // GPIO pin number for the alarm LED.

    volatile bool _running = false;                 // Flag indicating that moving is enabled (used in ISR).
    volatile bool _stopped = false;                 // Flag indicating that moving has ended (used in ISR).

    bool _calibrating = false;                      // Flag indicating that the calibration routine is running.
    bool _calibrated  = false;                      // Flag indicating that the calibration has been completed.
    bool _enabled     = false;                      // Flag indicating that the motor is enabled.
    bool _limit       = false;                      // Flag indicating that a limit switch has turned on.
    bool _alarm       = false;                      // Flag indicating that the stepper driver alarm has been turned on.
    bool _verbose     = false;                      // Flag indicating verbose output turned on.

    Direction _direction = Direction::CW;           // Stepper driver direction (CW: 1, CCW: -1).

    float         _distancePerRotation = 8.0f;      // Distance per full rotation (mm).
    ushort        _stepsPerRotation    = 200;       // Steps per full rotation (360°).
    ushort        _microsteps          = 1;         // Stepper driver microstep settings.
    float         _minspeed            = 2000.0f;   // The minimum stepper speed in steps per second.
    float         _maxspeed            = 5000.0f;   // The maximum stepper speed in steps per second.
    float         _maxsteps            = 1.0f;      // The number of steps for a ramp from minimum to maximum speed.

    long          _rampsteps  = 0;                  // The number of steps for ramping.
    float         _deltaspeed = 0.0f;               // The speed delta for every step.
    float         _speed = 0.0f;                    // The current speed (steps per second).

    long          _position = 0;                    // Absolute stepper position (steps).
    long          _target   = 0;                    // Absolute target position (steps).
    long          _steps    = 0;                    // Number of total steps requested in move.
    long          _n        = 0;                    // Step counter.

    unsigned long _start   = 0;                     // Time at start of move (millis).
    float         _elapsed = 0;                     // Elapsed time for last move (seconds).

    String _getTimeUTC();                           // Get the current time (UTC) as a string.
    void   _ccw();                                  // Turn off the direction pin.
    void   _cw();                                   // Turn on the direction pin.

    float  _getSpeedFromIntervals(uint intervals);  // Convert the intervals to speed.
    uint   _getIntervalsFromSpeed(float speed);     // Convert the speed to intervals.
           
    float  _getSpeedFromRPM(float speed);           // Convert the RPM in speed (steps per second).
    float  _getRPMFromSpeed(float speed);           // Convert the speed (steps per second) in RPM.
           
    long   _getStepsFromDistance(float vlue);       // Convert distance [mm] to steps.
    float  _getDistanceFromSteps(long value);       // Convert steps to distance [mm].
           
    bool   _isValidMicrostep(ushort valu);          // Returns true if microstep value is valid.

public : 
    float     getRPM();                             // Gets the current speed in RPM.
    float     getSpeed();                           // Gets the current speed in steps per second.
    float     getElapsed();                         // Gets the current elapsed time in seconds.
    float     getPercentage();                      // Gets the current percentage of the move.
    float     getMinSpeed();                        // Gets the minimum speed in steps per second.
    String    setMinSpeed(float value);             // Sets the minimum speed in steps per second.
    float     getMaxSpeed();                        // Gets the maximum speed in steps per second.
    String    setMaxSpeed(float value);             // Sets the maximum speed in steps per second.
    long      getMaxSteps();                        // Gets the ramp steps to maximum speed.
    String    setMaxSteps(long value);              // Sets the ramp steps to maximum speed.
    ushort    getMicrosteps();                      // Gets the microsteps for the stepper driver.
    String    setMicrosteps(ushort value);          // Sets the microsteps for the stepper driver.
    long      getPosition();                        // Gets the current position in steps.
    long      getDelta();                           // Gets the remaining steps to the target position.
    long      getTarget();                          // Gets the target position in steps.
    String    setTarget(long value);                // Sets the target position in steps.
    float     getDistance();                        // Gets the current position in mm.
    Direction getDirection();                       // Gets the current direction.
    float     getRetract();                         // Gets the retract distance in mm.
    String    setRetract(float value);              // Sets the retract distance in mm.
    float     getMinStep();                         // Gets the minimum step distance in mm.
    String    setMinStep(float value);              // Sets the minimum step distance in mm.
    float     getSmallStep();                       // Gets the small step distance in mm.
    String    setSmallStep(float value);            // Sets the small step distance in mm.

    bool getEnabledFlag();                          // True if acceleration and deceleration have been enabled.
    bool getRunningFlag();                          // True if the stepper is running (position != target).
    bool getLimitFlag();                            // True if the stepper has hit a limit switch.
    bool getAlarmFlag();                            // True if the stepper alarm signal is on.
    bool getCalibratingFlag();                      // True if calibrating.
    bool getCalibratedFlag();                       // True if calibration was successful.

    void init();                                    // Initialize the stepper instance.
    void update();                                  // Update stepper settings with current values.
    void enable();                                  // Enables the stepper outputs.
    void disable();                                 // Disables the stepper outputs.
    void stop();                                    // Stop moving (resetting target position, disable output).

    String home();                                  // Move to position zero (home).
    String reset();                                 // Reset the current position to zero.
    String calibrate();                             // Run the calibration routine.

    String getMoveInfo();                           // Return move info.
    String moveAway();                              // Retract a short distance in the opposite direction.
    String moveTrack(uint8_t value);                // Move to specified track (0..9).
    String moveAbsolute(long value);                // Move to absolute position [steps].
    String moveRelative(long value);                // Move relative distance [steps].
    String moveAbsoluteDistance(float value);       // Move to absolute position [mm].
    String moveRelativeDistance(float value);       // Move relative distance [mm].

    void alarmOn(uint8_t pin);                      // Alarm callback routine (on event).
    void alarmOff(uint8_t pin);                     // Alarm callback routine (off event).

    void switchOn(uint8_t pin);                     // Switch callback routine (on event).
    void switchOff(uint8_t pin);                    // Switch callback routine (off event).

    void onTimer();                                 // Delay timer callback routine.

    String toJsonString();                          // Get a serialized JSON representation.
    String toString();                              // Get a string representation.
};




