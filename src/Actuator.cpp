// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Actuator.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>10-5-2023 6:48 AM</created>
// <modified>20-5-2023 7:34 AM</modified>
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
        _minspeed = max(MIN_SPEED, min(MAX_SPEED, value));
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
    return abs(_target - _position);
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
/// Gets the retract distance in mm.
/// </summary>
/// <returns>The retract distance.</returns>
float  LinearActuator::getRetract()
{
    return Settings.Actuator.Retract;
}

/// <summary>
/// Sets the retract distance in mm.
/// </summary>
/// <param name="value">The retract distance [mm].</param>
void   LinearActuator::setRetract(float value)
{
    Settings.Actuator.Retract = max(0, value);
    if (_verbose) Telnet.println(String("Retract distance set to ") + value);
}

/// <summary>
/// Gets the minimum step distance in mm.
/// </summary>
/// <returns>The minimum step distance.</returns>
float  LinearActuator::getMinStep()
{
    return Settings.Actuator.MinStep;
}

/// <summary>
/// Sets the minimum step distance in mm.
/// </summary>
/// <param name="value">The minimum step distance [mm].</param>
void   LinearActuator::setMinStep(float value)
{
    Settings.Actuator.MinStep = max(0, value);
    if (_verbose) Telnet.println(String("Minimum step distance set to ") + value);
}

/// <summary>
/// Gets the small step distance in mm.
/// </summary>
/// <returns>The small step distance.</returns>
float  LinearActuator::getSmallStep()
{
    return Settings.Actuator.SmallStep;
}

/// <summary>
/// Sets the small step distance in mm.
/// </summary>
/// <param name="value">The small step distance [mm].</param>
void   LinearActuator::setSmallStep(float value)
{
    Settings.Actuator.SmallStep = max(0, value);
    if (_verbose) Telnet.println(String("Small step distance set to ") + value);
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
    return _running;
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
/// Updates the settings with current values.
/// </summary>
void LinearActuator::update()
{
    Settings.Actuator.Retract   = getRetract();
    Settings.Actuator.MinStep   = getMinStep();
    Settings.Actuator.SmallStep = getSmallStep();

    Settings.Stepper.MinSpeed   = getMinSpeed();
    Settings.Stepper.MaxSpeed   = getMaxSpeed();
    Settings.Stepper.MaxSteps   = getMaxSteps();
    Settings.Stepper.MicroSteps = getMicrosteps();
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
    // Clear the running flag.
    _running = false;
    digitalWrite(_PUL, LOW);

    // Clear the enabled flag.
    _enabled = false;
    digitalWrite(_ENA, HIGH);

    _target  = _position;
    _speed   = 0.0f;
    _steps   = 0;
    _start   = 0;
    _n = 0;

    // Clear the stopped flag.
    _stopped = false;
}


/// <summary>
/// Stops the move immediately. Resets the target to the current position.
/// </summary>
void LinearActuator::stop()
{
    // Get the elapsed time, clear the running flag and set the stopped flag.
    if (_running)
    {
        _elapsed = float(millis() - _start) / 1000.0;
        _running = false;
        _stopped = true;

        digitalWrite(_PUL, LOW);

        _target = _position;
        _speed = 0.0f;
        _steps = 0;
        _start = 0;
        _n = 0;
    }
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
    // Do not reset if still moving.
    if (getRunningFlag())
    {
        if (_verbose)
        {
            Telnet.println("Still moving - ignoring reset request");
        }

        return;
    }

    _n = 0;
    _speed = 0.0f;
    _steps = 0;
    _start = 0;
    _target   = 0;
    _position = 0;

    if (_verbose)
    {
        Telnet.print(String("Reset:") + "\r\n" +
                            "    Steps:       " + _steps    + "\r\n" +
                            "    Target:      " + _target   + "\r\n" +
                            "    Position:    " + _position + "\r\n" +
                            "\r\n");
    }

    // Clear the stopped flag.
    _stopped = false;
}

/// <summary>
/// Retracts a short distance in the opposite direction.
/// </summary>
void LinearActuator::moveAway()
{
    Direction direction = getDirection();
    moveRelativeDistance((-1.0f) * Settings.Actuator.Retract * float(static_cast<int>(direction)));
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

    // Set target and number of steps.
    _target = value;
    _steps = abs(_target - _position);

    // Calculate the delta speed from max and min speed values.
    _deltaspeed = (_maxspeed - _minspeed) / _maxsteps;

    long  conststeps = 0;       // The number of steps with constant (maximum) speed. 

    float maxtime = 0.0f;       // The time for a complete ramp from minimum to maximum speed.
    float ramptime = 0.0f;      // The time for a partial ramp from minimum speed.
    float maxspeed = 0.0f;      // The maximum speed for the partial ramp.
    float consttime = 0.0f;     // The time for the constant speed part.
    float totaltime = 0.0f;     // The total time moving the stepper motor (steps).
    float acceleration = 0.0f;  // The acceleration (speed / second) for the ramp.


    // Calculate the actual ramping steps.
    if (2 * _maxsteps > _steps)
    {
        _rampsteps = _steps / 2;
        conststeps = 0;
    }
    else
    {
        _rampsteps = _maxsteps;
        conststeps = _steps - 2 * _rampsteps;
    }

    // Calculate the time for the complete ramp.
    for (int i = 0; i < _maxsteps; i++)
    {
        maxtime += 1 / (_minspeed + i * _deltaspeed);
    }

    if (_rampsteps < 4)
    {
        if (_verbose)
        {
            Telnet.println("Requested move distance too small - ignoring move request");
        }

        return;
    }

    // Calculate the ramp time and maximum speed for the actual ramp.
    if (_rampsteps < _maxsteps)
    {
        maxspeed = _minspeed + _rampsteps * _deltaspeed;

        for (int i = 0; i < _rampsteps; i++)
        {
            ramptime += 1 / (_minspeed + i * _deltaspeed);
        }
    }
    else
    {
        maxspeed = _maxspeed;
        ramptime = maxtime;
    }

    // Calculate the constant speed time and total time.
    consttime = conststeps / _maxspeed;
    totaltime = 2 * ramptime + consttime;
    acceleration = (maxspeed - _minspeed) / ramptime;

    // First check for direction and delay if changing.
    if (_position < _target)
    {
        if (_direction != LinearActuator::Direction::CW)
        {
            _cw();
            sleep_ms(DIR_DELAY);
        }
    }
    else if (_position > _target)
    {
        if (_direction != LinearActuator::Direction::CCW)
        {
            _ccw();
            sleep_ms(DIR_DELAY);
        }
    }

    if (_verbose)
    {
        if (_rampsteps < _maxsteps)
        {
            Telnet.print(String("Move Info:") + "\r\n" +
                "    Position (steps):  " + _position    + "\r\n" +
                "    Target (steps):    " + _target      + "\r\n" +
                "    Total Steps:       " + _steps       + "\r\n" +
                "    Direction:         " + _direction   + "\r\n" +
                "    Min. Speed:        " + _minspeed    + "\r\n" +
                "    Max. Speed:        " + _maxspeed    + "\r\n" +
                "    Delta Speed:       " + _deltaspeed  + "\r\n" +
                "    Ramp Steps (max):  " + _maxsteps    + "\r\n" +
                "    Ramp Steps:        " + _rampsteps   + "\r\n" +
                "    Ramp Speed (max):  " + maxspeed     + "\r\n" +
                "    Ramp Time (max):   " + maxtime      + "\r\n" +
                "    Ramp Time:         " + ramptime     + "\r\n" +
                "    Acceleration:      " + acceleration + "\r\n" +
                "    Total Time:        " + totaltime    + "\r\n" +
                "\r\n");
        }
        else
        {
            Telnet.print(String("Move Info:") + "\r\n" +
                "    Position (steps):  " + _position    + "\r\n" +
                "    Target (steps):    " + _target      + "\r\n" +
                "    Total Steps:       " + _steps       + "\r\n" +
                "    Direction:         " + _direction   + "\r\n" +
                "    Min. Speed:        " + _minspeed    + "\r\n" +
                "    Max. Speed:        " + _maxspeed    + "\r\n" +
                "    Delta Speed:       " + _deltaspeed  + "\r\n" +
                "    Ramp Steps (max):  " + _maxsteps    + "\r\n" +
                "    Ramp Time (max):   " + maxtime      + "\r\n" +
                "    Acceleration:      " + acceleration + "\r\n" +
                "    Const Speed Steps: " + conststeps   + "\r\n" +
                "    Const Speed Time:  " + consttime    + "\r\n" +
                "    Total Time:        " + totaltime    + "\r\n" +
                "\r\n");
        }
    }

    // Get start time and set the running flag and clear the stop flag...
    _n = 0;
    _elapsed = 0;
    _start = millis();
    _stopped = false;
    _running = true;
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
    stop();

    // The stop switch has been turned on (disable stepper motor).
    if (pin == Settings.Actuator.SwitchStop)
    {
        disable();
    }
    // If a limit switch has been hit, move away (retract).
    else
    {
        _limit = true;
        moveAway();
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

        // If calibrating, reset current position to new home position (zero).
        if (_calibrating)
        {
            reset();
            _calibrated = true;
            _calibrating = false;
        }
    }
    else if (pin == Settings.Actuator.SwitchLimit2)
    {
        _limit = false;

        // If calibrating, stop calibrating (this should not happen).
        if (_calibrating)
        {
            _calibrated = false;
            _calibrating = false;
        }
    }
}

/// <summary>
/// Start the calibration routine by moving in negative direction (actuator length).
/// Eventually the first limit switch should be engaged near the home position.
/// </summary>
void LinearActuator::calibrate()
{
    _calibrating = true;
    moveRelativeDistance(-Settings.Actuator.Length);
}

/// <summary>
/// Timer callback - move a single step if not yet at target. The timer is triggered every 10 microseconds (100 kHz).
/// This ISR routine has to be as short as possible (less than 10 microseconds) in order to be called repeatedly.
/// Note that two flags (_running and _stopped are used to indicate the movement status to the main program.
/// </summary>
void LinearActuator::onTimer()
{
    static long  count;     // Counter for delay between steps.
    static long  intervals; // Number of intervals between steps.

    // // Generate stepper driver output pulses only if the running flag is set.
    if (_running)
    {
        // If the target has not yet been reached generate stepper pulse.
        if (_position != _target)
        {
            // Start pulse at the first interval and update speed and delay intervals.
            if (count == 0)
            {
                digitalWrite(_PUL, HIGH);

                // Calculate speed from step count.
                if (_n <= _rampsteps) _speed = _minspeed + _n * _deltaspeed;
                else if (_n >= (_steps - _rampsteps)) _speed = _minspeed + (_steps - _n) * _deltaspeed;

                intervals = _getIntervalsFromSpeed(_speed);
            }

            // Turn output low (end pulse) update step count and position.
            if (count == 1)
            {
                digitalWrite(_PUL, LOW);
                ++_n;

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
            // If the move has finished set start time to zero, reset move parameter, 
            // clear the running flag and set the stopped flag.
            if (_start > 0)
            {
                _running = false;
                _stopped = true;
                _elapsed = float(millis() - _start) / 1000.0;
                intervals = 0;
                _speed = 0.0f;
                _start = 0;
                _n = 0;
            }
        }

        // Increment interval count (delay between steps).
        ++count;
        
        // When the interval count has been reached, reset count and start again.
        if (count >= intervals)
        {
            count = 0;
        }
    }
}

/// <summary>
/// Print move info on Telnet. This is done when the ISR routine indicates that the move has stopped.
/// </summary>
void LinearActuator::info()
{
    // Print elapsed time (sec) when verbose output is enabled.
    if (_stopped)
    {
        if (_verbose)
        {
            Telnet.println(String("Moving time: ") + _elapsed + " sec (" + _steps + " steps)");
            Telnet.print(Settings.Telnet.Prompt);
        }

        // Reset the stopped flag.
        _stopped = false;
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