﻿// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>1-5-2023 7:18 AM</modified>
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

    long  Delta;                // The remaining steps from the current position to the target position.
    long  Steps;                // The current motor position in steps.
    long  Target;               // The target position in steps. Positive is clockwise from the 0 position.

    int   Direction;            // The current stepper direction (CW: 1, CCW: -1).
    float Position;             // The current motor position in mm.
    long  Delay;                // The current pulse delay in microseconds.
    float Speed;                // The current speed in steps per second.
    float SpeedRPM;             // The current motor speed in RPM.
    long  Microsteps;           // The currently configured microsteps.

    String toJsonString();      // Get a serialized JSON representation.
    String toString();          // Get a string representation.
};
