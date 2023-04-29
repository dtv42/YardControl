// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>29-4-2023 9:43 PM</modified>
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
extern RPI_PICO_Timer Timer;

/// <summary>
/// Initialize the stepper instance using the application settings. 
/// Register global callbacks, and setup debounce switches. Finally enable the outputs.
/// </summary>
void LinearActuator::init()
{
    _PUL = Settings.Stepper.PinPUL;
    _DIR = Settings.Stepper.PinDIR;
    _ENA = Settings.Stepper.PinENA;

    _interval = Settings.Stepper.Interval;
    _microsteps = Settings.Stepper.MicroSteps;
    _stepsPerRotation = Settings.Stepper.StepsPerRotation;
    _distancePerRotation = Settings.Stepper.DistancePerRotation;

    _position = 0;
    _target = 0;
    _count = 0;
}

void      LinearActuator::_ccw()                            { digitalWrite(_DIR, LOW); _direction = LinearActuator::Direction::CCW; }
void      LinearActuator::_cw()                             { digitalWrite(_DIR, HIGH); _direction = LinearActuator::Direction::CW; }
     
uint      LinearActuator::getCount()                        { return _count; }
uint      LinearActuator::getInterval()                     { return _interval; }
void      LinearActuator::setInterval(uint value)           { _interval = value; }
ushort    LinearActuator::getMicroSteps()                   { return _microsteps; }
void      LinearActuator::setMicroSteps(ushort value)       { _microsteps = value; }
long      LinearActuator::getSteps()                        { return _position; }
long      LinearActuator::getTarget()                       { return _target; }
long      LinearActuator::getStepsToGo()                    { return _target - _position; }
float     LinearActuator::getPosition()                     { return  (_distancePerRotation * _position) / (_stepsPerRotation * _microsteps); }

LinearActuator::Direction LinearActuator::getDirection()    { return _direction; }
bool      LinearActuator::getEnabledFlag()                  { return _enabledFlag; }
bool      LinearActuator::getRunningFlag()                  { return _target != _position; }
bool      LinearActuator::getLimitFlag()                    { return _limitFlag; }
bool      LinearActuator::getAlarmFlag()                    { return _alarmFlag; }
bool      LinearActuator::getCalibratingFlag()              { return _calibratingFlag; }
bool      LinearActuator::getCalibratedFlag()               { return _calibratedFlag; }

void      LinearActuator::home()                            { _target = 0; }
void      LinearActuator::reset()                           { _position = 0; _target = 0; _count = 0; }

void      LinearActuator::moveAbsolute(long value)          { _target = value; }
void      LinearActuator::moveRelative(long value)          { _target = _position + value; }
void      LinearActuator::moveAbsoluteDistance(float value) { _target = value * (_stepsPerRotation * _microsteps) / _distancePerRotation; }
void      LinearActuator::moveRelativeDistance(float value) { _target = _position + (value * (_stepsPerRotation * _microsteps) / _distancePerRotation); }

void LinearActuator::enable()
{
    digitalWrite(_ENA, LOW);
    delayMicroseconds(200);
    _enabledFlag = true;
    _count = 0;
}

void LinearActuator::disable()
{
    digitalWrite(_ENA, HIGH);
    delayMicroseconds(200);
    _enabledFlag = false;
    _count = 0;
}

/// <summary>
/// Retract a short distance.
/// </summary>
void LinearActuator::retract(long value)
{
    if ((value == 1) || (value == -1))
    {
        moveRelativeDistance(value * Settings.Actuator.Retract);
    }
}

/// <summary>
/// Stops the motor immediately by disabling the outputs.
/// The stepper target is set to the current position.
/// </summary>
void LinearActuator::stop()
{
    _calibratingFlag = false;
    _target = _position;
    _count = 0;
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
        retract(1);
    }
    // If the second limit switch has been hit retract (negative direction).
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _limitFlag = true;
        retract(-1);
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
        _target = _position;

        // If calibrating reset current position to new home position (zero).
        if (_calibratingFlag)
        {
            _target = 0;
            _position = 0;
            _calibratedFlag = true;
            _calibratingFlag = false;
        }
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _limitFlag = false;
        _target = _position;

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
        // Turn output low (end pulse) and update position.
        else if (_count == 2)
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
        // If the total interval count has been reached reset count.
        else if (_count > _interval)
        {
            _count = 0;
        }
    }
}
