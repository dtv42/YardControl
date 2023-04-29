// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>28-4-2023 8:53 PM</modified>
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
    ConstantSpeedFlag = Actuator.getConstantSpeedFlag();    // Flag indicating running with constant speed.
    CalibratedFlag    = Actuator.getCalibratedFlag();       // Flag indicating the calibration has been completed.
    CalibratingFlag   = Actuator.getCalibratingFlag();      // Flag indicating the calibration routine is running.
    EnabledFlag       = Actuator.getEnabledFlag();          // Flag indicating the stepper motor is enabled.
    RunningFlag       = Actuator.getRunningFlag();          // Flag indicating that stepper motor is moving.
    LimitFlag         = Actuator.getLimitFlag();            // Flag indicating that a limit switch has been hit.
    AlarmFlag         = Actuator.getAlarmFlag();            // Flag indicating that the stepper alarm has been turned on.
    Steps             = Actuator.getSteps();                // The current motor position in steps.
    Target            = Actuator.getTarget();               // The target position in steps. Positive is clockwise from the 0 position.
    StepsToGo         = Actuator.getStepsToGo();            // The remaining steps from the current position to the target position.
    Position          = Actuator.getPosition();             // The current motor position in mm.
    Speed             = Actuator.getSpeed();                // The current speed in steps per second.
    MaxSpeed          = Actuator.getMaxSpeed();             // The currently configured maximum speed.
    Acceleration      = Actuator.getAcceleration();         // The currently configured acceleration/deceleration.
    Microsteps        = Actuator.getMicroSteps();           // The currently configured microsteps.
    Pulsewidth        = Actuator.getPulseWidth();           // The currently configured minimum pulsewidth.
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String ActuatorInfo::toJsonString()
{
    String json;

    _doc.clear();
    _doc["ConstantSpeedFlag"] = ConstantSpeedFlag;
    _doc["CalibratedFlag"]    = CalibratedFlag;
    _doc["CalibratingFlag"]   = CalibratingFlag;
    _doc["EnabledFlag"]       = EnabledFlag;
    _doc["RunningFlag"]       = RunningFlag;
    _doc["LimitFlag"]         = LimitFlag;
    _doc["AlarmFlag"]         = AlarmFlag;
    _doc["Steps"]             = Steps;
    _doc["Target"]            = Target;
    _doc["StepsToGo"]         = StepsToGo;
    _doc["Position"]          = Position;
    _doc["Speed"]             = Speed;
    _doc["MaxSpeed"]          = MaxSpeed;
    _doc["Acceleration"]      = Acceleration;
    _doc["Microsteps"]        = Microsteps;
    _doc["Pulsewidth"]        = Pulsewidth;
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
                  "    ConstantSpeedFlag: " + ConstantSpeedFlag + "\r\n" +
                  "    CalibratedFlag:    " + CalibratedFlag    + "\r\n" +
                  "    CalibratingFlag:   " + CalibratingFlag   + "\r\n" +
                  "    EnabledFlag:       " + EnabledFlag       + "\r\n" +
                  "    RunningFlag:       " + RunningFlag       + "\r\n" +
                  "    LimitFlag:         " + LimitFlag         + "\r\n" +
                  "    AlarmFlag:         " + AlarmFlag         + "\r\n" +
                  "    Steps:             " + Steps             + "\r\n" +
                  "    Target:            " + Target            + "\r\n" +
                  "    StepsToGo:         " + StepsToGo         + "\r\n" +
                  "    Position:          " + Position          + "\r\n" +
                  "    Speed:             " + Speed             + "\r\n" +
                  "    MaxSpeed:          " + MaxSpeed          + "\r\n" +
                  "    Acceleration:      " + Acceleration      + "\r\n" +
                  "    Microsteps:        " + Microsteps        + "\r\n" +
                  "    Pulsewidth:        " + Pulsewidth        + "\r\n" +
                  "\r\n";
}