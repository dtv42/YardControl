// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>6-5-2023 7:59 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include "ActuatorInfo.h"
#include "Actuator.h"

/// <summary>
/// Used for initialization of the actuator info.
/// </summary>
extern LinearActuator Actuator;

/// <summary>
///  Using the global Actuator instance to get the actual data.
/// </summary>
ActuatorInfo::ActuatorInfo()
{
    CalibratedFlag  = Actuator.getCalibratedFlag();     // Flag indicating that the calibration routine is running.
    CalibratingFlag = Actuator.getCalibratingFlag();    // Flag indicating that the calibration has been completed.
    EnabledFlag     = Actuator.getEnabledFlag();        // Flag indicating that the motor is enabled.
    RunningFlag     = Actuator.getRunningFlag();        // Flag indicating that the motor is running.
    RampingFlag     = Actuator.getRampingFlag();        // Flag indicating that acceleration/deceleration is performed.
    LimitFlag       = Actuator.getLimitFlag();          // Flag indicating that a limit switch has turned on.
    AlarmFlag       = Actuator.getAlarmFlag();          // Flag indicating that the stepper driver alarm has been turned on.
    VerboseFlag     = Actuator.getVerboseFlag();        // Flag indicating that verbose output has been turned on.
    Delta           = Actuator.getDelta();              // The remaining steps from the current position to the target position.
    Steps           = Actuator.getSteps();              // The current motor position in steps.
    Target          = Actuator.getTarget();             // The target position in steps.
    Direction       = Actuator.getDirection();          // The current stepper direction (CW: 1, CCW: -1).
    Position        = Actuator.getPosition();           // The current motor position in mm.
    Delay           = Actuator.getDelay();              // The current pulse delay in microseconds.
    RPM             = Actuator.getRPM();                // The current speed in RPM.
    Speed           = Actuator.getSpeed();              // The current speed in steps per second.
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String ActuatorInfo::toJsonString()
{
    String json;

    _doc.clear();
    _doc["CalibratingFlag"] = CalibratingFlag;
    _doc["CalibratedFlag"]  = CalibratedFlag;
    _doc["EnabledFlag"]     = EnabledFlag;
    _doc["RunningFlag"]     = RunningFlag;
    _doc["RampingFlag"]     = RampingFlag;
    _doc["LimitFlag"]       = LimitFlag;
    _doc["AlarmFlag"]       = AlarmFlag;
    _doc["VerboseFlag"]     = VerboseFlag;
    _doc["Delta"]           = Delta;
    _doc["Steps"]           = Steps;
    _doc["Target"]          = Target;
    _doc["Direction"]       = Direction;
    _doc["Position"]        = Position;
    _doc["Delay"]           = Delay;
    _doc["RPM"]             = RPM;
    _doc["Speed"]           = Speed;
    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Returns a printable string representation of the actuator info.
/// </summary>
/// <returns>The printable string.</returns>
String ActuatorInfo::toString()
{
    return String("Actuator Info:") + "\r\n" +
                  "    CalibratingFlag: " + CalibratingFlag + "\r\n" +
                  "    CalibratedFlag:  " + CalibratedFlag  + "\r\n" +
                  "    EnabledFlag:     " + EnabledFlag     + "\r\n" +
                  "    RunningFlag:     " + RunningFlag     + "\r\n" +
                  "    RampingFlag:     " + RampingFlag     + "\r\n" +
                  "    LimitFlag:       " + LimitFlag       + "\r\n" +
                  "    AlarmFlag:       " + AlarmFlag       + "\r\n" +
                  "    VerboseFlag:     " + VerboseFlag     + "\r\n" +
                  "    Delta:           " + Delta           + "\r\n" +
                  "    Steps:           " + Steps           + "\r\n" +
                  "    Target:          " + Target          + "\r\n" +
                  "    Direction:       " + Direction       + "\r\n" +
                  "    Position:        " + Position        + "\r\n" +
                  "    Delay:           " + Delay           + "\r\n" +
                  "    RPM:             " + RPM             + "\r\n" +
                  "    Speed:           " + Speed           + "\r\n" +
                  "\r\n";
}