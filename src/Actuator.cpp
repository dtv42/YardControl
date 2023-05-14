﻿// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>10-5-2023 6:48 AM</created>
// <modified>14-5-2023 12:38 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <ArduinoJson.h>

#include "Actuator.h"
#include "Commands.h"
#include "AppSettings.h"
#include "TelnetServer.h"

// Externals (globals) and callback routines.
extern AppSettings Settings;
extern TelnetServer Telnet;
extern CommandsClass Commands;
extern LinearActuator Actuator;
extern bool TimerHandler(struct repeating_timer* t);

/// <summary>
/// Set the stepper driver direction to counter clockwise.
/// </summary>
void LinearActuator::_ccw()
{
    digitalWrite(_DIR, HIGH);
    _direction = LinearActuator::Direction::CCW;
}

/// <summary>
/// Set the stepper driver direction to clockwise.
/// </summary>
void LinearActuator::_cw()
{
    digitalWrite(_DIR, LOW);
    _direction = LinearActuator::Direction::CW;
}

/// <summary>
/// Gets the speed in steps per second from number of (10 usec) intervals.
/// Note that the speed is kept within the minimum and maximum speed range.
/// </summary>
/// <param name="value">The number of intervals.</param>
/// <returns>The speed in steps per second.</returns>
float LinearActuator::_getSpeedFromIntervals(uint value)
{
    float speed = (value > 0) ? float(LinearActuator::FREQUENCY / value) : 0;
    return max(LinearActuator::FREQUENCY / INT_MAX, min(LinearActuator::MAX_SPEED, speed));
}

/// <summary>
/// Gets the number of (10 usec) intervals from the speed in steps per seconds.
/// </summary>
/// <param name="value">The speed in steps per second.</param>
/// <returns>The number of intervals.</returns>
uint LinearActuator::_getIntervalsFromSpeed(float value)
{
    uint intervals = (value > 0) ? uint(LinearActuator::FREQUENCY / value) : 0;
    return max(2, min(INT_MAX, intervals));
}

/// <summary>
/// Gets the speed in RPM from speed in steps per seconds.
/// </summary>
/// <param name="value">The speed in steps per second.</param>
/// <returns>The speed in RPM.</returns>
float LinearActuator::_getRPMFromSpeed(float value)
{
    return 60.0 * value / (_microsteps * _stepsPerRotation);
}

/// <summary>
/// Gets the speed speed in steps per seconds from speed in RPM.
/// </summary>
/// <param name="value">The speed in RPM.</param>
/// <returns>The speed in steps per second.</returns>
float LinearActuator::_getSpeedFromRPM(float value)
{
    return value * (_microsteps * _stepsPerRotation) / 60.0;
}

/// <summary>
/// Gets the number of steps from distance in mm.
/// </summary>
/// <param name="value">The distance [mm].</param>
/// <returns>The number of steps.</returns>
long LinearActuator::_getStepsFromDistance(float value)
{
    return long(value * (_stepsPerRotation * _microsteps) / _distancePerRotation);
}

/// <summary>
/// Gets the distance in mm from the number of steps.
/// </summary>
/// <param name="value">The number of steps.</param>
/// <returns>The distance [mm].</returns>
float LinearActuator::_getDistanceFromSteps(long value)
{
    return float(value * _distancePerRotation / (_stepsPerRotation * _microsteps));
}

/// <summary>
/// Checks the input value against allowed microstep values.
/// </summary>
/// <param name="value">The microstep value.</param>
/// <returns>True if microstep value is valid.</returns>
bool LinearActuator::_isValidMicrostep(ushort value)
{
    switch (value)
    {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
    case 5:
    case 10:
    case 20:
    case 25:
    case 40:
    case 50:
    case 100:
    case 125:
        return true;
    default:
        return false;
    }
}

/// <summary>
/// Updates the settings with current values.
/// </summary>
void LinearActuator::_updateSettings()
{
    Settings.Stepper.MaxSpeed   = getMaxSpeed();
    Settings.Stepper.MaxSteps   = getMaxSteps();
    Settings.Stepper.MicroSteps = getMicrosteps();
}

/// <summary>
/// Gets the current speed in RPM.
/// </summary>
/// <returns>The speed [RPM].</returns>
float  LinearActuator::getRPM()
{
    return _getRPMFromSpeed(_speed);
}

/// <summary>
/// Gets the current speed in steps per second.
/// </summary>
/// <returns>The speed [steps per second].</returns>
float  LinearActuator::getSpeed()
{
    return _speed;
}


/// <summary>
/// Gets the minimum speed.
/// </summary>
/// <returns>The minimum speed.</returns>
float  LinearActuator::getMinSpeed()
{
    return _minspeed;
}

/// <summary>
/// Sets the minimum speed in steps per second.
/// </summary>
/// <param name="value">The minimum speed [steps per second].</param>
void   LinearActuator::setMinSpeed(float value)
{
    if (getRunningFlag())
    {
        if (_verbose) Telnet.println("Still moving - ignoring set minimum speed request");
    }
    else
    {
        _maxspeed = max(MIN_SPEED, min(MAX_SPEED, value));
        if (_verbose) Telnet.println(String("Minimum speed set to ") + _maxspeed);
    }
}

/// <summary>
/// Gets the maximum speed.
/// </summary>
/// <returns>The maximum speed.</returns>
float  LinearActuator::getMaxSpeed()
{
    return _maxspeed;
}

/// <summary>
/// Sets the maximum speed in steps per second.
/// </summary>
/// <param name="value">The maximum speed [steps per second].</param>
void   LinearActuator::setMaxSpeed(float value)
{
    if (getRunningFlag())
    {
        if (_verbose) Telnet.println("Still moving - ignoring set maximum speed request");
    }
    else
    {
        _maxspeed = max(MIN_SPEED, min(MAX_SPEED, value));
        if (_verbose) Telnet.println(String("Maximum speed set to ") + _maxspeed);
    }
}

/// <summary>
/// Gets the ramp steps to maximum speed.
/// </summary>
/// <returns>The ramp steps.</returns>
long LinearActuator::getMaxSteps()
{
    return _maxsteps;
}

/// <summary>
/// Sets the ramp steps to maximum speed.
/// </summary>
/// <param name="value">The ramp steps.</param>
void LinearActuator::setMaxSteps(long value)
{
    if (getRunningFlag())
    {
        if (_verbose) Telnet.println("Still moving - ignoring set maximum steps request");
    }
    else
    {
        _maxsteps = max(1, value);
        if (_verbose) Telnet.println(String("Maximum steps set to ") + _maxsteps);
    }
}

/// <summary>
/// Gets the microsteps.
/// </summary>
/// <returns>The microsteps.</returns>
ushort  LinearActuator::getMicrosteps()
{
    return _microsteps;
}

/// <summary>
/// Sets the microsteps.
/// </summary>
/// <param name="value">The microsteps.</param>
void   LinearActuator::setMicrosteps(ushort value)
{
    if (getRunningFlag())
    {
        if (_verbose) Telnet.println("Still moving - ignoring set microsteps request");
    }
    else
    {
        if (_isValidMicrostep(value))
        {
            _microsteps = max(1, value);
            if (_verbose) Telnet.println(String("Microsteps set to ") + _microsteps);
        }
        else
        {
            if (_verbose) Telnet.println(String("Invalid microsteps value: ") + _microsteps);
        }
    }
}

/// <summary>
/// Gets the current position in steps.
/// </summary>
/// <returns>The current position.</returns>
long   LinearActuator::getPosition()
{
    return _position;
}

/// <summary>
/// Gets the remaining steps to reach the target.
/// </summary>
/// <returns>The number of steps remaining.</returns>
long   LinearActuator::getDelta()
{
    return _steps;
}

/// <summary>
/// Gets the current target position in steps.
/// </summary>
/// <returns>The current target.</returns>
long   LinearActuator::getTarget()
{
    return _target;
}


/// <summary>
/// Sets the target in steps.
/// </summary>
/// <param name="value">The new target.</param>
void   LinearActuator::setTarget(long value)
{
    _target = value;

    if (_verbose) Telnet.println(String("Target set to ") + _target);
}

/// <summary>
/// Gets the current position in mm.
/// </summary>
/// <returns>The position [mm].</returns>
float  LinearActuator::getDistance()
{
    return  _getDistanceFromSteps(_position);
}
                        
/// <summary>
/// Gets the current stepper driver direction (CW: 1, CCW: -1).
/// </summary>
/// <returns>The direction.</returns>
LinearActuator::Direction LinearActuator::getDirection()
{
    return _direction;
}

/// <summary>
/// Gets the verbose flag.
/// </summary>
/// <returns>The flag value.</returns>
bool LinearActuator::getVerboseFlag()
{
    return _verbose;
}

/// <summary>
/// Sets the verbose flag.
/// </summary>
/// <param name="value">The new flag value.</param>
/// <returns></returns>
void LinearActuator::setVerboseFlag(bool value)
{
    _verbose = value;
    if (_verbose) Telnet.println("Verbose on"); else Telnet.println("Verbose off");
}

/// <summary>
/// Gets the enabled flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getEnabledFlag()
{
    return _enabled;
}

/// <summary>
/// Gets the running flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getRunningFlag()
{
    return _position != _target;
}

/// <summary>
/// Gets the limit flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getLimitFlag()
{
    return _limit;
}

/// <summary>
/// Gets the alarm flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getAlarmFlag()
{
    return _alarm;
}

/// <summary>
/// Gets the calibrating flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getCalibratingFlag()
{
    return _calibrating;
}

/// <summary>
/// Gets the calibrated flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getCalibratedFlag()
{
    return _calibrated;
}

/// <summary>
/// Initialize the stepper instance using the application settings.
/// Enable the driver and allow acceleration and deceleration.
/// </summary>
void LinearActuator::init()
{
    // Set output and inut pin numbers.
    _PUL = Settings.Stepper.PinPUL;
    _DIR = Settings.Stepper.PinDIR;
    _ENA = Settings.Stepper.PinENA;
    _ALM = Settings.Stepper.PinALM;

    // Set stepper settings.
    setMinSpeed(Settings.Stepper.MinSpeed);
    setMaxSpeed(Settings.Stepper.MaxSpeed);
    setMaxSteps(Settings.Stepper.MaxSteps);
    setMicrosteps(Settings.Stepper.MicroSteps);

    // Set fixed stepper settings.
    if (Settings.Stepper.StepsPerRotation > 0) _stepsPerRotation = Settings.Stepper.StepsPerRotation;
    if (Settings.Stepper.DistancePerRotation > 0) _distancePerRotation = Settings.Stepper.DistancePerRotation;

    // Reset position and enable outputs.
    reset();
    enable();
}

/// <summary>
/// Enable the stepper driver outputs.
/// </summary>
void LinearActuator::enable()
{
    digitalWrite(_ENA, LOW);
    _enabled = true;
}

/// <summary>
/// Disables the stepper driver outputs, stops moving and resets the target to current position.
/// </summary>
void LinearActuator::disable()
{
    sleep_us(10);
    digitalWrite(_ENA, HIGH);
    digitalWrite(_PUL, LOW);
    _target = _position;
    _steps = 0;
    _start = 0;
    _enabled = false;
}


/// <summary>
/// Stops the move.
/// </summary>
void LinearActuator::stop()
{
    sleep_us(10);
    digitalWrite(_PUL, LOW);
    _target = _position;
    _steps = 0;
    _start = 0;
}

/// <summary>
/// Sets the target to zero (home).
/// </summary>
void   LinearActuator::home()
{
    moveAbsolute(0);
}

/// <summary>
/// Resets the move parameter, the target, and the position to zero.
/// </summary>
void   LinearActuator::reset()
{
    _start     = 0;
    _changing  = 0;
    _intervals = 1;
    _count     = 0;

    _target    = 0;
    _position  = 0;
    _steps     = 0;
    _speed     = 0.0f;

    if (_verbose)
    {
        Telnet.print(String("Reset:") + "\r\n" +
                            "    Speed:       " + _speed    + "\r\n" +
                            "    Steps:       " + _steps    + "\r\n" +
                            "    Target:      " + _target   + "\r\n" +
                            "    Position:    " + _position + "\r\n" +
                            "\r\n");
    }
}

/// <summary>
/// Retracts a short distance (CW: 1, CCW: -1).
/// </summary>
void LinearActuator::retract(Direction value)
{
    moveRelativeDistance(value * Settings.Actuator.Retract);
}

/// <summary>
/// Sets the target to the specified number of steps.
/// The counters are set to zero. Other move parameters are initialized.
/// </summary>
/// <param name="value">The number of steps.</param>
void   LinearActuator::moveAbsolute(long value)
{
    // Do not move if still moving.
    if (getRunningFlag())
    {
        if (_verbose)
        {
            Telnet.println("Still moving - ignoring move request");
        }

        return;
    }

    long  n = 0;
    long  count = 0;
    float speed = 0.0f;
    float deltaspeed = 0.0f;

    long  target = value;
    long  position = getPosition();
    long  steps = abs(target - position);

    long  rampsteps = 0;
    long  conststeps = 0;
    float maxtime = 0.0f;
    float ramptime = 0.0f;
    float consttime = 0.0f;
    float totaltime = 0.0f;

    deltaspeed = (_maxspeed - _minspeed) / _maxsteps;

    if (2 * _maxsteps > steps)
    {
        rampsteps = steps / 2;
        conststeps = 0;
    }
    else
    {
        rampsteps = _maxsteps;
        conststeps = steps - 2 * rampsteps;
    }

    for (int i = 1; i <= _maxsteps; ++i)
    {
        if (i == 1) maxtime = 1 / _minspeed;
        else maxtime += 1 / (i * _deltaspeed + _minspeed);
    }

    for (int i = 1; i <= rampsteps; ++i)
    {
        if (i == 1) ramptime = 1 / _minspeed;
        else ramptime += 1 / (i * _deltaspeed + _minspeed);
    }

    consttime = conststeps / _maxspeed;
    totaltime = 2 * ramptime + consttime;

    _rampsteps = rampsteps;
    _target = target;
    _steps = steps;
    _speed = 0.0f;
    _count = 0;
    _n = 0;

    if (_verbose)
    {
        Telnet.print(String("Move Info:") + "\r\n" +
                            "    Position (steps): " + position    + "\r\n" +
                            "    Target (steps):   " + target      + "\r\n" +
                            "    Total Steps:      " + steps       + "\r\n" +
                            "    Min. Speed:       " + _minspeed   + "\r\n" +
                            "    Max. Speed:       " + _maxspeed   + "\r\n" +
                            "    Ramp Steps (max): " + _maxsteps   + "\r\n" +
                            "    Ramp Steps:       " + rampsteps   + "\r\n" +
                            "    Max. Speed Steps: " + conststeps  + "\r\n" +
                            "    Delta Speed:      " + _deltaspeed + "\r\n" +
                            "    Ramp Time (max):  " + maxtime     + "\r\n" +
                            "    Ramp Time:        " + ramptime    + "\r\n" +
                            "    Max. Speed Steps: " + conststeps  + "\r\n" +
                            "    Max. Speed Time:  " + consttime   + "\r\n" +
                            "    Total Time:       " + totaltime   + "\r\n" +
                            "\r\n");
    }

    _start = millis();

    interrupts();
}

/// <summary>
/// Sets the target relative to the current position.
/// </summary>
/// <param name="value">The number of steps.</param>
void   LinearActuator::moveRelative(long value)
{
    moveAbsolute(_position + value);
}

/// <summary>
/// Sets the target to the specified distance [mm].
/// </summary>
/// <param name="value">The distance [mm].</param>
void   LinearActuator::moveAbsoluteDistance(float value)
{
    moveAbsolute(_getStepsFromDistance(value));
}

/// <summary>
/// Sets the target relative to the current position.
/// </summary>
/// <param name="value">The distance [mm].</param>
void   LinearActuator::moveRelativeDistance(float value)
{
    moveRelative(_getStepsFromDistance(value));
}

/// <summary>
/// Callback routine for the stepper alarm on event (over voltage or over current).
/// The stepper motor is stopped (disabled).
/// </summary>
void LinearActuator::alarmOn(uint8_t pin)
{
    _alarm = true;
    disable();
}

/// <summary>
/// Callback routine for the stepper alarm off event.
/// </summary>
void LinearActuator::alarmOff(uint8_t pin)
{
    _alarm = false;
    enable();
}

/// <summary>
/// Callback routine for the switches (pressedCallback).
/// </summary>
void LinearActuator::switchOn(uint8_t pin)
{
    // The stop switch has been turned on.
    if (pin == Settings.Actuator.SwitchStop)
    {
        _calibrated = false;
        stop();
    }
    // If the first limit switch has been hit retract (positive direction).
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        _limit = true;
        stop();
        retract(Direction::CW);
    }
    // If the second limit switch has been hit retract (negative direction).
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _limit = true;
        stop();
        retract(Direction::CCW);
    }
}

/// <summary>
/// Callback routine for the switches (releasedCallback).
/// </summary>
void LinearActuator::switchOff(uint8_t pin)
{
    if (pin == Settings.Actuator.SwitchStop)
    {
        enable();
    }
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        _limit = false;
        moveAbsolute(_steps);

        // If calibrating reset current position to new home position (zero).
        if (_calibrating)
        {
            reset();
            _calibrated = true;
            _calibrating = false;
        }
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        // If calibrating stop calibrating (this should not happen).
        if (_calibrating)
        {
            _calibrated = false;
            _calibrating = false;
        }
    }
}

/// <summary>
/// Start the calibration routine by moving a large negative position (actuator length).
/// Eventually the first limit switch should be engaged near the home position.
/// </summary>
void LinearActuator::calibrate()
{
    moveRelativeDistance(-Settings.Actuator.Length);
    _calibrating = true;
}

/// <summary>
/// Timer callback - move a single step if not yet at target.
/// The timer is triggered every 10 microseconds (100 kHz).
/// </summary>
void LinearActuator::onTimer()
{
    // If the direction is changing just return.
    if (_changing > 0)
    {
        --_changing;
        return;
    }

    // // Generate stepper driver output pulses only if enabled.
    if (_enabled)
    {
        // If the target has not yet been reached generate stepper pulse.
        if (_position != _target)
        {
            // First check for direction and start direction change delay if necessary.
            if (_count == 0)
            {
                if (_position < _target)
                {
                    if (_direction != LinearActuator::Direction::CW)
                    {
                        _cw();
                        _changing = CHANGING;
                        return;
                    }
                }
                else if (_position > _target)
                {
                    if (_direction != LinearActuator::Direction::CCW)
                    {
                        _ccw();
                        _changing = CHANGING;
                        return;
                    }
                }
            }

            // Increment interval count (delay between steps).
            ++_count;

            // Start pulse at the first interval and increment step count.
            if (_count == 1)
            {
                digitalWrite(_PUL, HIGH);
                ++_n;

                // Calculate speed from step count.
                if (_n < _rampsteps) _speed = _minspeed + _n * _deltaspeed;
                else if (_n > (_steps - _rampsteps)) _speed = _minspeed + (_rampsteps - _n) * _deltaspeed;
                else _speed = _maxspeed;

                _intervals = _getIntervalsFromSpeed(_speed);
            }

            // Turn output low (end pulse) update speed, intervals, and position.
            if (_count == 2)
            {
                digitalWrite(_PUL, LOW);

                if (_position < _target)
                {
                    ++_position;
                }
                else if (_position > _target)
                {
                    --_position;
                }
            }
        }
        else
        {
            // If the move has finished set start time to zero and reset move parameter.
            if (_start > 0)
            {
                // Print elapsed time (sec) when verbose output is enabled.
                if (_verbose)
                {
                    auto elapsed = float(millis() - _start) / 1000.0;
                    Telnet.println(String("Moving time: ") + elapsed + " sec");
                }

                _intervals = 0;
                _speed = 0.0f;
                _start = 0;
                _n = 0;
            }
        }

        // When the interval count has been reached, reset count and start again.
        if (_count >= _intervals)
        {
            _count = 0;
        }
    }
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String LinearActuator::toJsonString()
{
    String json;

    _doc.clear();
    _doc["Calibrating"] = getCalibratingFlag();
    _doc["Calibrated"]  = getCalibratedFlag();
    _doc["Verbose"]     = getVerboseFlag();
    _doc["Enabled"]     = getEnabledFlag();
    _doc["Running"]     = getRunningFlag();
    _doc["Limit"]       = getLimitFlag();
    _doc["Alarm"]       = getAlarmFlag();
    _doc["Delta"]       = getDelta();
    _doc["Target"]      = getTarget();
    _doc["Position"]    = getPosition();
    _doc["Distance"]    = getDistance();
    _doc["Direction"]   = getDirection();
    _doc["RPM"]         = getRPM();
    _doc["Speed"]       = getSpeed();
    _doc["MinSpeed"]    = getMinSpeed();
    _doc["MaxSpeed"]    = getMaxSpeed();
    _doc["MaxSteps"]    = getMaxSteps();
    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Returns a printable string representation of the actuator info.
/// </summary>
/// <returns>The printable string.</returns>
String LinearActuator::toString()
{
    return String("Actuator Status:") + "\r\n" +
                  "    Calibrating: " + getCalibratingFlag() + "\r\n" +
                  "    Calibrated:  " + getCalibratedFlag()  + "\r\n" +
                  "    Verbose:     " + getVerboseFlag()     + "\r\n" +
                  "    Enabled:     " + getEnabledFlag()     + "\r\n" +
                  "    Running:     " + getRunningFlag()     + "\r\n" +
                  "    Limit:       " + getLimitFlag()       + "\r\n" +
                  "    Alarm:       " + getAlarmFlag()       + "\r\n" +
                  "    Delta:       " + getDelta()           + "\r\n" +
                  "    Target:      " + getTarget()          + "\r\n" +
                  "    Position:    " + getPosition()        + "\r\n" +
                  "    Distance:    " + getDistance()        + "\r\n" +
                  "    Direction:   " + getDirection()       + "\r\n" +
                  "    RPM:         " + getRPM()             + "\r\n" +
                  "    Speed:       " + getSpeed()           + "\r\n" +
                  "    MinSpeed:    " + getMinSpeed()        + "\r\n" +
                  "    MaxSpeed:    " + getMaxSpeed()        + "\r\n" +
                  "    MaxSteps:    " + getMaxSteps()        + "\r\n" +
                  "\r\n";
}