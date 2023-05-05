﻿// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>5-5-2023 11:12 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
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
/// Initialize the stepper instance using the application settings.
/// Enable the driver and allow acceleration and deceleration.
/// </summary>
void LinearActuator::init()
{
    _PUL = Settings.Stepper.PinPUL;
    _DIR = Settings.Stepper.PinDIR;
    _ENA = Settings.Stepper.PinENA;
    _ALM = Settings.Stepper.PinALM;

    setMaxSpeed(Settings.Stepper.MaxSpeed);
    setConstSpeed(Settings.Stepper.ConstSpeed);
    setAcceleration(Settings.Stepper.Acceleration);
    setMicrosteps(Settings.Stepper.MicroSteps);
    _stepsPerRotation = max(1, Settings.Stepper.StepsPerRotation);
    _distancePerRotation = max(1.0, Settings.Stepper.DistancePerRotation);

    reset();
    enable();
    rampEnable();
}

/// <summary>
/// Set the stepper driver direction to counter clockwise.
/// </summary>
void LinearActuator::_ccw()
{
    digitalWrite(_DIR, LOW);
    _direction = LinearActuator::Direction::CCW;
}

/// <summary>
/// Set the stepper driver direction to clockwise.
/// </summary>
void LinearActuator::_cw()
{
    digitalWrite(_DIR, HIGH);
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
    return max(float(LinearActuator::FREQUENCY / INT_MAX), min(float(LinearActuator::MAXSPEED), speed));
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
float  LinearActuator::_getRPMFromSpeed(float value)
{
    return 60.0 * value / (_microsteps * _stepsPerRotation);
}

/// <summary>
/// Gets the speed speed in steps per seconds from speed in RPM.
/// </summary>
/// <param name="value">The speed in RPM.</param>
/// <returns>The speed in steps per second.</returns>
float  LinearActuator::_getSpeedFromRPM(float value)
{
    return value * (_microsteps * _stepsPerRotation) / 60.0;
}

/// <summary>
/// Gets the number of steps from distance in mm.
/// </summary>
/// <param name="value">The distance [mm].</param>
/// <returns>The number of steps.</returns>
long   LinearActuator::_getStepsFromDistance(float value)
{
    return long(value * (_stepsPerRotation * _microsteps) / _distancePerRotation);
}

/// <summary>
/// Gets the distance in mm from the number of steps.
/// </summary>
/// <param name="value">The number of steps.</param>
/// <returns>The distance [mm].</returns>
float  LinearActuator::_getDistanceFromSteps(long value)
{
    return float(value * _distancePerRotation / (_stepsPerRotation * _microsteps));
}

/// <summary>
/// Converts the speed to the delay in microseconds.
/// </summary>
/// <returns>The delay [usec].</returns>
float  LinearActuator::getDelay()
{
    return  _getIntervalsFromSpeed(_speed) * LinearActuator::INTERVAL;
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
/// Gets the maximum speed.
/// </summary>
/// <returns>The minimum speed.</returns>
float  LinearActuator::getMinSpeed()
{
    return _minSpeed;
}

/// <summary>
/// Sets the minimum speed in steps per second.
/// Note this also updates the settings.
/// </summary>
/// <param name="value">The maximum speed [steps per second].</param>
void   LinearActuator::setMinSpeed(float value)
{
    _minSpeed = min(_maxSpeed, max(MINSPEED, value));
    Settings.Stepper.MinSpeed = _minSpeed;
}

/// <summary>
/// Gets the maximum speed.
/// </summary>
/// <returns>The maximum speed.</returns>
float  LinearActuator::getMaxSpeed()
{
    return _maxSpeed;
}

/// <summary>
/// Sets the maximum speed in steps per second.
/// Note this also updates the settings.
/// </summary>
/// <param name="value">The maximum speed [steps per second].</param>
void   LinearActuator::setMaxSpeed(float value)
{
    _maxSpeed = min(MAXSPEED, max(_minSpeed, value));
    Settings.Stepper.MaxSpeed = _maxSpeed;
}

/// <summary>
/// Gets the constant speed.
/// </summary>
/// <returns>The constant speed.</returns>
float  LinearActuator::getConstSpeed()
{
    return _constSpeed;
}

/// <summary>
/// Sets the constant speed in steps per second.
/// Note this also updates the settings.
/// </summary>
/// <param name="value">The constant speed [steps per second].</param>
void   LinearActuator::setConstSpeed(float value)
{
    _constSpeed = min(MAXSPEED, max(MINSPEED, value));
    Settings.Stepper.ConstSpeed = _constSpeed;
}

/// <summary>
/// Gets the acceleration in speed per second.
/// </summary>
/// <returns>The acceleration.</returns>
float  LinearActuator::getAcceleration()
{
    return _acceleration;
}

/// <summary>
/// Sets the acceleration in speed per second.
/// Note this also updates the settings.
/// </summary>
/// <param name="value">The acceleration.</param>
void   LinearActuator::setAcceleration(float value)
{
    _acceleration = min(MAXSPEED, max(0, value));
    Settings.Stepper.Acceleration = _acceleration;
}

/// <summary>
/// Gets the stepper driver microsteps.
/// </summary>
/// <returns>The number of microsteps.</returns>
ushort LinearActuator::getMicrosteps()
{
    return _microsteps;
}

/// <summary>
/// Sets the stepper driver microsteps.
/// Note this also updates the settings.
/// The stepper driver DIP switches have to be set accordingly.
/// </summary>
/// <param name="value">The number of microsteps.</param>
void   LinearActuator::setMicrosteps(ushort value)
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
        _microsteps = value;
    }

    Settings.Stepper.MicroSteps = _microsteps;
}

/// <summary>
/// Gets the current position in steps.
/// </summary>
/// <returns>The current steps.</returns>
long   LinearActuator::getSteps()
{
    return _position;
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
/// Gets the remaining steps to reach the target.
/// </summary>
/// <returns>The number of steps remaining.</returns>
long   LinearActuator::getDelta()
{
    return _steps;
}

/// <summary>
/// Gets the current position in mm.
/// </summary>
/// <returns>The position [mm].</returns>
float  LinearActuator::getPosition()
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
/// Gets the enabled flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getEnabledFlag()
{
    return _enabledFlag;
}

/// <summary>
/// Gets the running flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getRunningFlag()
{
    return _steps > 0;
}

/// <summary>
/// Gets the constant speed flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getConstFlag()
{
    return _constFlag;
}

/// <summary>
/// Gets the limit flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getLimitFlag()
{
    return _limitFlag;
}

/// <summary>
/// Gets the alarm flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getAlarmFlag()
{
    return _alarmFlag;
}

/// <summary>
/// Gets the calibrating flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getCalibratingFlag()
{
    return _calibratingFlag;
}

/// <summary>
/// Gets the calibrated flag.
/// </summary>
/// <returns>The flag value.</returns>
bool   LinearActuator::getCalibratedFlag()
{
    return _calibratedFlag;
}
            
/// <summary>
/// Sets the target to zero (home).
/// </summary>
void   LinearActuator::home()
{
    moveAbsolute(0);
}

/// <summary>
/// Resets the interval counter, the target, the steps, and the position to zero.
/// </summary>
void   LinearActuator::reset()
{
    _count = 0;
    _steps = 0;
    _target = 0;
    _position = 0;
}

/// <summary>
/// Sets the target to the specified number of steps.
/// The counters are set to zero. Other move parameters are initialized.
/// </summary>
/// <param name="value">The number of steps.</param>
void   LinearActuator::moveAbsolute(long value)
{
    _count = 0;
    _speed = 0;
    _target = value;
    _steps = abs(_target - _position);
    _totalSteps = _steps;
    Serial.print("Steps: "); Serial.println(_steps); Serial.flush();

    if (_constFlag)
    {
        _speed = _constSpeed;
        _intervals = _getIntervalsFromSpeed(_speed);
        Serial.print("Total Steps: "); Serial.println(_totalSteps); Serial.flush();
        Serial.print("Speed:       "); Serial.println(_speed); Serial.flush();
    }
    else
    {
        _rampSteps = (_maxSpeed - _minSpeed) * (_maxSpeed - _minSpeed) / _acceleration;
        _deltaSpeed = (_maxSpeed - _minSpeed) / _rampSteps;
        _speed = _minSpeed;

        // Adjust ramp count if ramping takes longer than total steps.
        if ((2 * _rampSteps) > _totalSteps)
        {
            _rampSteps = _totalSteps / 2;
            _constSteps = 0;
        }
        else
        {
            _constSteps = _totalSteps - 2 * _rampSteps;
        }
        Serial.print("Total Steps: "); Serial.println(_totalSteps); Serial.flush();
        Serial.print("Const Steps: "); Serial.println(_constSteps); Serial.flush();
        Serial.print("Ramps Steps: "); Serial.println(_rampSteps); Serial.flush();
        Serial.print("Delta Speed: "); Serial.println(_deltaSpeed); Serial.flush();
        Serial.print("Speed:       "); Serial.println(_speed); Serial.flush();
    }
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
/// Enables constant speed mode (disables acceleration and deceleration).
/// </summary>
void LinearActuator::rampEnable()
{
    _constFlag = false;
    _speed = _minSpeed;
}

/// <summary>
/// Disables constant speed mode (enables acceleration and deceleration).
/// </summary>
void LinearActuator::rampDisable()
{
    _constFlag = true;
    _speed = _constSpeed;
}

/// <summary>
/// Enable the stepper driver outputs and resets the target to current position.
/// </summary>
void LinearActuator::enable()
{
    digitalWrite(_ENA, LOW);
    delayMicroseconds(200);
    moveAbsolute(_position);
    _enabledFlag = true;
}

/// <summary>
/// Disables the stepper driver outputs and the target to current position.
/// </summary>
void LinearActuator::disable()
{
    digitalWrite(_ENA, HIGH);
    digitalWrite(_PUL, LOW);
    delayMicroseconds(200);
    moveAbsolute(_position);
    _enabledFlag = false;
}

/// <summary>
/// Retracts a short distance (CW: 1, CCW: -1).
/// </summary>
void LinearActuator::retract(Direction value)
{
    moveRelativeDistance(value * Settings.Actuator.Retract);
}

/// <summary>
/// Stops the motor immediately by disabling the outputs.
/// If calibration is running.
/// </summary>
void LinearActuator::stop()
{
    _calibratingFlag = false;
    disable();
}

/// <summary>
/// Callback routine for the stepper alarm on event (over voltage or over current).
/// The stepper motor is stopped (disabled).
/// </summary>
void LinearActuator::alarmOn(uint8_t pin)
{
    _alarmFlag = true;
    stop();
}

/// <summary>
/// Callback routine for the stepper alarm off event.
/// </summary>
void LinearActuator::alarmOff(uint8_t pin)
{
    _alarmFlag = false;
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
        _calibratedFlag = false;
        stop();
    }
    // If the first limit switch has been hit retract (positive direction).
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        _limitFlag = true;
        retract(Direction::CW);
    }
    // If the second limit switch has been hit retract (negative direction).
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _limitFlag = true;
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
        _limitFlag = false;
        moveAbsolute(_steps);

        // If calibrating reset current position to new home position (zero).
        if (_calibratingFlag)
        {
            reset();
            rampEnable();
            _calibratedFlag = true;
            _calibratingFlag = false;
        }
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        moveAbsolute(_steps);

        // If calibrating stop calibrating (this should not happen).
        if (_calibratingFlag)
        {
            _calibratedFlag = false;
            _calibratingFlag = false;
        }
    }
}

/// <summary>
/// Start the calibration routine by moving a large negative position (actuator length).
/// Eventually the first limit switch should be engaged near the home position.
/// </summary>
void LinearActuator::calibrate()
{
    rampDisable();
    moveRelativeDistance(-Settings.Actuator.Length);
    _calibratingFlag = true;
}

/// <summary>
/// Timer callback - move a single step if not yet at target.
/// The timer is triggered every 10 microseconds (100 kHz).
/// </summary>
void LinearActuator::onTimer()
{
    // Action only if enabled.
    if (_enabledFlag)
    {
        ++_count;
        
        // If just started check the direction and restart count if new direction detected. Otherwise start pulse.
        if (_count == 1)
        {
            if (_position < _target)
            {
                if (_direction != LinearActuator::Direction::CW)
                {
                    _cw();
                    _count = 0;
                }
                else
                {
                    digitalWrite(_PUL, HIGH);
                }
            }
            else if (_position > _target)
            {
                if (_direction != LinearActuator::Direction::CCW)
                {
                    _ccw();
                    _count = 0;
                }
                else
                {
                    digitalWrite(_PUL, HIGH);
                }
            }
        }
        // Turn output low (end pulse) and update remaining steps and position.
        else if (_count == 2)
        {
            if (_steps > 0)
            {
                digitalWrite(_PUL, LOW);
                --_steps;

                if (_position < _target)
                {
                    ++_position;
                }
                else if (_position > _target)
                {
                    --_position;
                }

                if (!_constFlag)
                {
                    if (_steps > _totalSteps - _rampSteps)
                    {
                        _speed += _deltaSpeed;
                    }
                    else if (_steps < _rampSteps)
                    {
                        _speed -= _deltaSpeed;
                    }
                    else
                    {
                        _speed = _maxSpeed;
                    }
                    _intervals = _getIntervalsFromSpeed(_speed);
                }
            }
            else
            {
                if (!_constFlag)
                {
                    _speed = _minSpeed;
                }
                else
                {
                    _speed = _constSpeed;
                }
            }
        }
        // If the total interval count has been reached (delay according to current speed) reset count.
        else
        {
            if (_count >= _intervals)
                _count = 0;
        }
    }
}
