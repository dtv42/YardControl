// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>10-4-2023 11:33 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "Actuator.h"
#include "Commands.h"
#include "AppSettings.h"
#include "TelnetServer.h"

// Define the input switch debounce delay [ms].
#define SWITCH_DEBOUNCE_DELAY 10
#define CALIBRATION_TARGET -100

// Externals (globals) and callback routines.
extern AppSettings Settings;
extern TelnetServer Telnet;
extern CommandsClass Commands;

extern void pressedCallback(uint8_t pin);
extern void releasedCallback(uint8_t pin);

/// <summary>
/// Initialize the stepper instance. Register global callbacks, and setup debounce switches.
/// </summary>
void LinearActuator::init()
{
    _stepper = AccelStepperWithDistance(AccelStepper::DRIVER, Settings.Stepper.PinPUL, Settings.Stepper.PinDIR);
    _stepper.setEnablePin(Settings.Stepper.PinENA);
    _stepper.setMaxSpeed(Settings.Stepper.MaxSpeed);
    _stepper.setAcceleration(Settings.Stepper.Acceleration);
    _stepper.setMicroStep(Settings.Stepper.MicroSteps);
    _stepper.setStepsPerRotation(Settings.Stepper.StepsPerRotation);
    _stepper.setDistancePerRotation(Settings.Stepper.DistancePerRotation);
    _stepper.setMinPulseWidth(Settings.Stepper.MinPulseWidth);
    _stepper.setPinsInverted(false, false, true);

    _switchStop.registerCallbacks(pressedCallback, releasedCallback, NULL, NULL);
    _switchLimit1.registerCallbacks(pressedCallback, releasedCallback, NULL, NULL);
    _switchLimit2.registerCallbacks(pressedCallback, releasedCallback, NULL, NULL);

    _switchStop.setup  (Settings.Actuator.SwitchStop,   SWITCH_DEBOUNCE_DELAY, InputDebounce::PIM_INT_PULL_UP_RES);
    _switchLimit1.setup(Settings.Actuator.SwitchLimit1, SWITCH_DEBOUNCE_DELAY, InputDebounce::PIM_INT_PULL_UP_RES);
    _switchLimit2.setup(Settings.Actuator.SwitchLimit2, SWITCH_DEBOUNCE_DELAY, InputDebounce::PIM_INT_PULL_UP_RES);
}

float LinearActuator::getSpeed()                        { return _stepper.speed(); }
void  LinearActuator::setSpeed(float value)             { _stepper.setSpeed(value); }
float LinearActuator::getMaxSpeed()                     { return _stepper.maxSpeed(); }
void  LinearActuator::setMaxSpeed(float value)          { _stepper.setMaxSpeed(value); }
float LinearActuator::getAcceleration()                 { return _stepper.acceleration(); }
void  LinearActuator::setAcceleration(float value)      { _stepper.setAcceleration(value); }
long  LinearActuator::getSteps()                        { return _stepper.currentPosition(); }
long  LinearActuator::getTarget()                       { return _stepper.targetPosition(); }
long  LinearActuator::getStepsToGo()                    { return _stepper.distanceToGo(); }
float LinearActuator::getPosition()                     { return _stepper.getCurrentPositionDistance(); }
bool  LinearActuator::isEnabled()                       { return digitalRead(Settings.Stepper.PinENA) == LOW; }
bool  LinearActuator::isRunning()                       { return _stepper.isRunning(); }
bool  LinearActuator::isStopped()                       { return _isStopped; }
bool  LinearActuator::isCalibrating()                   { return _isCalibrating; }
bool  LinearActuator::isCalibrated()                    { return _isCalibrated; }
void  LinearActuator::enable()                          { _stepper.enableOutputs(); }
void  LinearActuator::disable()                         { _stepper.disableOutputs(); }
void  LinearActuator::home()                            { _stepper.moveTo(0); }
void  LinearActuator::reset()                           { _stepper.setCurrentPosition(0); home(); }
void  LinearActuator::moveAbsolute(long value)          { _stepper.moveTo(value); }
void  LinearActuator::moveRelative(long value)          { _stepper.move(value); }
void  LinearActuator::moveAbsoluteDistance(float value) { _stepper.moveToDistance(value); }
void  LinearActuator::moveRelativeDistance(float value) { _stepper.moveRelative(value); }

/// <summary>
/// The main update routine - called from loop().
/// Update indicator leds and run the stepper.
/// </summary>
void LinearActuator::run()
{
    digitalWrite(Settings.Actuator.LedRunning, _stepper.isRunning() ? HIGH : LOW);
    digitalWrite(Settings.Actuator.LedStop, _isStopped ? HIGH : LOW);
    _isCalibrating ? _stepper.runSpeed() : _stepper.run();
}

/// <summary>
/// Stops the motor immediately by disabling the outputs.
/// The stepper target is set to the current position.
/// </summary>
void LinearActuator::stop()
{
    _isStopped = true;
    _stepper.disableOutputs();
    _stepper.setSpeed(0);
    _stepper.moveTo(_stepper.currentPosition());
}

/// <summary>
/// Release the stepper motor from a stop by enabling the outputs.
/// </summary>
void LinearActuator::release()
{
    _isStopped = false;
    _stepper.setSpeed(0);
    _stepper.enableOutputs();
}

/// <summary>
/// Callback routine for the switches (pressedCallback).
/// </summary>
void LinearActuator::switchOn(uint8_t pin)
{
    if (pin == Settings.Actuator.SwitchStop)
    {
        _isStopped = true;
        _isCalibrating = false;
        _isCalibrated = false;
        _stepper.disableOutputs();
        _stepper.setSpeed(0);
        _stepper.moveTo(_stepper.currentPosition());
    }
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        digitalWrite(Settings.Actuator.LedLimit, HIGH);
        _stepper.moveRelative(Settings.Actuator.Retract);
        _stepper.setSpeed(Settings.Actuator.MoveSpeed);
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        digitalWrite(Settings.Actuator.LedLimit, HIGH);
        _stepper.moveRelative(-Settings.Actuator.Retract);
        _stepper.setSpeed(Settings.Actuator.MoveSpeed);
        _stepper.runSpeed();
        _stepper.setCurrentPosition(0);
        _stepper.setSpeed(0);
    }
}

/// <summary>
/// Callback routine for the switches (releasedCallback).
/// </summary>
void LinearActuator::switchOff(uint8_t pin)
{
    if (pin == Settings.Actuator.SwitchStop)
    {
        // Do nothing. Release stop has to be triggered through Http or Telnet.
    }
    else if (pin == Settings.Actuator.SwitchLimit1)
    {
        digitalWrite(Settings.Actuator.LedLimit, LOW);

        _stepper.setSpeed(0);
        _stepper.move(0);

        if (_isCalibrating)
        {
            _stepper.setCurrentPosition(0);
            _isCalibrated = true;
            _isCalibrating = false;
        }
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        digitalWrite(Settings.Actuator.LedLimit, LOW);

        _stepper.setSpeed(0);
        _stepper.move(0);

        if (_isCalibrating)
        {
            _isCalibrated = false;
            _isCalibrating = false;
        }
    }
}

/// <summary>
/// Start the calibration routine by moving to a negative position.
/// </summary>
void LinearActuator::calibrate()
{
    _stepper.moveToDistance(CALIBRATION_TARGET);
    _isCalibrating = true;
}
