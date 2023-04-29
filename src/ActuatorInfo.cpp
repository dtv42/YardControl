// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>29-4-2023 5:46 PM</modified>
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
    CalibratedFlag    = Actuator.getCalibratedFlag();       // Flag indicating the calibration has been completed.
    CalibratingFlag   = Actuator.getCalibratingFlag();      // Flag indicating the calibration routine is running.
    EnabledFlag       = Actuator.getEnabledFlag();          // Flag indicating the stepper motor is enabled.
    RunningFlag       = Actuator.getRunningFlag();          // Flag indicating that stepper motor is moving.
    LimitFlag         = Actuator.getLimitFlag();            // Flag indicating that a limit switch has been hit.
    AlarmFlag         = Actuator.getAlarmFlag();            // Flag indicating that the stepper alarm has been turned on.
    Count             = Actuator.getCount();                // The current interval count.
    Steps             = Actuator.getSteps();                // The current motor position in steps.
    Target            = Actuator.getTarget();               // The target position in steps. Positive is clockwise from the 0 position.
    StepsToGo         = Actuator.getStepsToGo();            // The remaining steps from the current position to the target position.
    Position          = Actuator.getPosition();             // The current motor position in mm.
    Interval          = Actuator.getInterval();             // The current timer interval in microseconds.
    Microsteps        = Actuator.getMicroSteps();           // The currently configured microsteps.
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
    _doc["CalibratedFlag"]    = CalibratedFlag;
    _doc["EnabledFlag"]       = EnabledFlag;
    _doc["RunningFlag"]       = RunningFlag;
    _doc["LimitFlag"]         = LimitFlag;
    _doc["AlarmFlag"]         = AlarmFlag;
    _doc["Count"]             = Count;
    _doc["Steps"]             = Steps;
    _doc["Target"]            = Target;
    _doc["StepsToGo"]         = StepsToGo;
    _doc["Position"]          = Position;
    _doc["Interval"]          = Interval;
    _doc["Microsteps"]        = Microsteps;
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
                  "    CalibratingFlag: " + CalibratingFlag   + "\r\n" +
                  "    CalibratedFlag:  " + CalibratedFlag    + "\r\n" +
                  "    EnabledFlag:     " + EnabledFlag       + "\r\n" +
                  "    RunningFlag:     " + RunningFlag       + "\r\n" +
                  "    LimitFlag:       " + LimitFlag         + "\r\n" +
                  "    AlarmFlag:       " + AlarmFlag         + "\r\n" +
                  "    Count:           " + Count             + "\r\n" +
                  "    Steps:           " + Steps             + "\r\n" +
                  "    Target:          " + Target            + "\r\n" +
                  "    StepsToGo:       " + StepsToGo         + "\r\n" +
                  "    Position:        " + Position          + "\r\n" +
                  "    Interval:        " + Interval          + "\r\n" +
                  "    Microsteps:      " + Microsteps        + "\r\n" +
                  "\r\n";
}