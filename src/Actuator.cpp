// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>28-4-2023 8:57 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "Actuator.h"
#include "Commands.h"
#include "AppSettings.h"
#include "TelnetServer.h"

// Externals (globals) and callback routines.
extern AppSettings Settings;
extern TelnetServer Telnet;
extern CommandsClass Commands;

/// <summary>
/// Initialize the stepper instance using the application settings. 
/// Register global callbacks, and setup debounce switches. Finally enable the outputs.
/// </summary>
void LinearActuator::init()
{
    _stepper = AccelStepperWithDistance(AccelStepper::DRIVER, Settings.Stepper.PinPUL, Settings.Stepper.PinDIR);
    _stepper.setEnablePin(Settings.Stepper.PinENA);
    _stepper.setMaxSpeed(Settings.Stepper.MaxSpeed);
    _stepper.setAcceleration(Settings.Stepper.Acceleration);
    _stepper.setStepsPerRotation(Settings.Stepper.StepsPerRotation);
    _stepper.setDistancePerRotation(Settings.Stepper.DistancePerRotation);
    _stepper.setPinsInverted(false, false, true);

    setMicroSteps(Settings.Stepper.MicroSteps);
    setPulseWidth(Settings.Stepper.MinPulseWidth);

    _stepper.enableOutputs();
}

float  LinearActuator::getSpeed()                        { return _stepper.speed(); }
void   LinearActuator::setSpeed(float value)             { _stepper.setSpeed(value); }
float  LinearActuator::getMaxSpeed()                     { return _stepper.maxSpeed(); }
void   LinearActuator::setMaxSpeed(float value)          { _stepper.setMaxSpeed(value); }
float  LinearActuator::getAcceleration()                 { return _stepper.acceleration(); }
void   LinearActuator::setAcceleration(float value)      { _stepper.setAcceleration(value); }
ushort LinearActuator::getPulseWidth()                   { return _pulsewidth;}
void   LinearActuator::setPulseWidth(ushort value)       { _stepper.setMinPulseWidth(value); _pulsewidth = value; }
ushort LinearActuator::getMicroSteps()                   { return _microsteps; }
void   LinearActuator::setMicroSteps(ushort value)       { _stepper.setMicroStep(value); _microsteps = value;}
long   LinearActuator::getSteps()                        { return _stepper.currentPosition(); }
long   LinearActuator::getTarget()                       { return _stepper.targetPosition(); }
long   LinearActuator::getStepsToGo()                    { return _stepper.distanceToGo(); }
float  LinearActuator::getPosition()                     { return _stepper.getCurrentPositionDistance(); }
bool   LinearActuator::getEnabledFlag()                  { return digitalRead(Settings.Stepper.PinENA) == LOW; }
bool   LinearActuator::getRunningFlag()                  { return _stepper.isRunning(); }
bool   LinearActuator::getLimitFlag()                    { return _getLimitFlag; }
bool   LinearActuator::getAlarmFlag()                    { return _getAlarmFlag; }
bool   LinearActuator::getCalibratingFlag()              { return _getCalibratingFlag; }
bool   LinearActuator::getCalibratedFlag()               { return _getCalibratedFlag; }
bool   LinearActuator::getConstantSpeedFlag()            { return _constantSpeed; }
void   LinearActuator::enable()                          { _stepper.enableOutputs(); }
void   LinearActuator::disable()                         { _stepper.disableOutputs(); }
void   LinearActuator::home()                            { _stepper.moveTo(0); }
void   LinearActuator::reset()                           { _stepper.setCurrentPosition(0); home(); }
void   LinearActuator::moveAbsolute(long value)          { _stepper.moveTo(value); }
void   LinearActuator::moveRelative(long value)          { _stepper.move(value); }
void   LinearActuator::moveAbsoluteDistance(float value) { _stepper.moveToDistance(value); }
void   LinearActuator::moveRelativeDistance(float value) { _stepper.moveRelative(value); }

/// <summary>
/// The main update routine - called from loop().
/// Update indicator leds and run the stepper.
/// </summary>
void LinearActuator::run()
{
    digitalWrite(Settings.Actuator.LedRunning, _stepper.isRunning() ? HIGH : LOW);
    digitalWrite(Settings.Actuator.LedInLimit, _getLimitFlag ? HIGH : LOW);
    digitalWrite(Settings.Actuator.LedAlarmOn, _getAlarmFlag ? HIGH : LOW);
    _constantSpeed ? _stepper.runSpeed() : _stepper.run();
}

/// <summary>
/// Retract a short distance.
/// </summary>
void LinearActuator::retract(long value)
{
    if ((value == 1) || (value == -1))
    {
        _stepper.moveRelative(value * Settings.Actuator.Retract);
        _stepper.setSpeed(Settings.Actuator.MoveSpeed);
        _constantSpeed = true;
    }
}

/// <summary>
/// Stops the motor immediately by disabling the outputs.
/// The stepper target is set to the current position.
/// </summary>
void LinearActuator::stop()
{
    _getCalibratingFlag = false;
    _stepper.disableOutputs();
    _stepper.setSpeed(0);
    _stepper.moveTo(_stepper.currentPosition());
}

/// <summary>
/// Release the stepper motor from a stop by enabling the outputs.
/// </summary>
void LinearActuator::release()
{
    _stepper.setSpeed(0);
    _stepper.enableOutputs();
}

/// <summary>
/// Callback routine for the stepper alarm on event (over voltage or over current).
/// </summary>
void LinearActuator::alarmOn(uint8_t pin)
{
    _getAlarmFlag = true;
    _stepper.disableOutputs();
    _stepper.setSpeed(0);
    _stepper.moveTo(_stepper.currentPosition());
}

/// <summary>
/// Callback routine for the stepper alarm off event.
/// </summary>
void LinearActuator::alarmOff(uint8_t pin)
{
    _getAlarmFlag = false;
    _stepper.enableOutputs();
}

/// <summary>
/// Callback routine for the switches (pressedCallback).
/// </summary>
void LinearActuator::switchOn(uint8_t pin)
{
    if (pin == Settings.Actuator.SwitchStop)
    {
        _getCalibratingFlag = false;
        _getCalibratedFlag = false;
        _stepper.disableOutputs();
        _stepper.setSpeed(0);
        _stepper.moveTo(_stepper.currentPosition());
    }
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        _getLimitFlag = true;
        retract(1);
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _getLimitFlag = true;
        retract(-1);
    }
}

/// <summary>
/// Callback routine for the switches (releasedCallback).
/// </summary>
void LinearActuator::switchOff(uint8_t pin)
{
    _constantSpeed = false;

    if (pin == Settings.Actuator.SwitchStop)
    {
        // Do nothing. Release stop has to be triggered through Http or Telnet.
    }
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        _getLimitFlag = false;
        _stepper.setSpeed(0);
        _stepper.move(0);

        if (_getCalibratingFlag)
        {
            _stepper.setCurrentPosition(0);
            _getCalibratingFlag = false;
            _getCalibratedFlag = true;
        }
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _getLimitFlag = false;
        _stepper.setSpeed(0);
        _stepper.move(0);

        if (_getCalibratingFlag)
        {
            _getCalibratedFlag = false;
            _getCalibratingFlag = false;
        }
    }
}

/// <summary>
/// Start the calibration routine by moving a large negative position (actuator length).
/// </summary>
void LinearActuator::calibrate()
{
    _stepper.moveRelative(-Settings.Actuator.Length);
    _stepper.setSpeed(Settings.Actuator.MoveSpeed);
    _constantSpeed = true;
    _getCalibratingFlag = true;
}

