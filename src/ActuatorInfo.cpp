// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>13-4-2023 1:46 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include "AppSettings.h"
#include "ActuatorInfo.h"
#include "Actuator.h"
#include "Defines.h"

/// <summary>
/// Used for initialization of the actuator info.
/// </summary>
extern LinearActuator Actuator;

/// <summary>
///  Using the global Actuator instance to get the actual data.
/// </summary>
ActuatorInfo::ActuatorInfo()
{
    IsCalibrated  = Actuator.isCalibrated();          // Flag indicating the calibration has been completed.
    IsCalibrating = Actuator.isCalibrating();         // Flag indicating the calibration routine is running.
    IsEnabled     = Actuator.isEnabled();             // Flag indicating the stepper motor is enabled.
    IsRunning     = Actuator.isRunning();             // Flag indicating that stepper motor is moving.
    IsInLimit     = Actuator.isInLimit();             // Flag indicating that a limit switch has been hit.
    IsAlarmOn     = Actuator.isAlarmOn();             // Flag indicating that the stepper alarm has been turned on.
    Steps         = Actuator.getSteps();              // The current motor position in steps.
    Target        = Actuator.getTarget();             // The target position in steps. Positive is clockwise from the 0 position.
    StepsToGo     = Actuator.getStepsToGo();          // The remaining steps from the current position to the target position.
    Position      = Actuator.getPosition();           // The current motor position in mm.
    Speed         = Actuator.getSpeed();              // The current speed in steps per second.
    MaxSpeed      = Actuator.getMaxSpeed();           // The currently configured maximum speed.
    Acceleration  = Actuator.getAcceleration();       // The currently configured acceleration/deceleration.
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String ActuatorInfo::toJsonString()
{
    String json;

    _doc.clear();
    _doc["IsCalibrated"]    = IsCalibrated;
    _doc["IsCalibrating"]   = IsCalibrating;
    _doc["IsEnabled"]       = IsEnabled;
    _doc["IsRunning"]       = IsRunning;
    _doc["IsInLimit"]       = IsInLimit;
    _doc["IsAlarmOn"]       = IsAlarmOn;
    _doc["Steps"]           = Steps;
    _doc["Target"]          = Target;
    _doc["StepsToGo"]       = StepsToGo;
    _doc["Position"]        = Position;
    _doc["Speed"]           = Speed;
    _doc["MaxSpeed"]        = MaxSpeed;
    _doc["Acceleration"]    = Acceleration;
    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Returns a printable string representation of the actuator info.
/// </summary>
/// <returns>The printable string.</returns>
String ActuatorInfo::toString()
{
    return String("Actuator Info:") + CRLF +
                  "    IsCalibrated:  " + IsCalibrated  + CRLF +
                  "    IsCalibrating: " + IsCalibrating + CRLF +
                  "    IsEnabled:     " + IsEnabled     + CRLF +
                  "    IsRunning:     " + IsRunning     + CRLF +
                  "    IsInLimit:     " + IsInLimit     + CRLF +
                  "    IsAlarmOn:     " + IsAlarmOn     + CRLF +
                  "    Steps:         " + Steps         + CRLF +
                  "    Target:        " + Target        + CRLF +
                  "    StepsToGo:     " + StepsToGo     + CRLF +
                  "    Position:      " + Position      + CRLF +
                  "    Speed:         " + Speed         + CRLF +
                  "    MaxSpeed:      " + MaxSpeed      + CRLF +
                  "    Acceleration:  " + Acceleration  + CRLF +
        CRLF;
}