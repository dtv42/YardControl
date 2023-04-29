// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>29-4-2023 9:47 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Arduino.h>

#define ARDIUNOJSON_TAB "    "

#include <ArduinoJson.h>

/// <summary>
/// This class holds the current stepper data.
/// </summary>
class ActuatorInfo
{
private:
    StaticJsonDocument<192> _doc; // The Json document representing the data.

public:
    ActuatorInfo();

    bool  CalibratingFlag;      // Flag indicating the calibration routine is running.
    bool  CalibratedFlag;       // Flag indicating the calibration has been completed.
    bool  EnabledFlag;          // Flag indicating if motor is enabled.
    bool  RunningFlag;          // Flag indicating if motor is running.
    bool  LimitFlag;            // Flag indicating that a limit switch has turned on.
    bool  AlarmFlag;            // Flag indicating that the stepper driver alarm has been turned on.

    long  Count;                // The current interval count.
    long  Steps;                // The current motor position in steps.
    long  Target;               // The target position in steps. Positive is clockwise from the 0 position.
    long  StepsToGo;            // The remaining steps from the current position to the target position.

    float Position;             // The current motor position in mm.
    long  Interval;             // The current timer interval in microseconds.
    long  Microsteps;           // The currently configured microsteps.

    String toJsonString();      // Get a serialized JSON representation.
    String toString();          // Get a string representation.
};
