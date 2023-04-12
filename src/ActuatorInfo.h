// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>12-4-2023 6:54 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "AccelStepperWithDistance.h"

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

    bool  IsCalibrated;     // Flag indicating the calibration has been completed.
    bool  IsCalibrating;    // Flag indicating the calibration routine is running.
    bool  IsEnabled;        // Flag indicating if motor is enabled.
    bool  IsRunning;        // Flag indicating if motor is running.
    bool  IsInLimit;        // Flag indicating that a limit switch has turned on.
    bool  IsAlarmOn;        // Flag indicating that the stepper driver alarm has been turned on.
    long  Steps;            // The current motor position in steps.
    long  Target;           // The target position in steps. Positive is clockwise from the 0 position.
    long  StepsToGo;        // The remaining steps from the current position to the target position.
    float Position;         // The current motor position in mm.
    float Length;           // The length of the linear actuator in mm.
    float Speed;            // The current speed in steps per second.
    float MaxSpeed;         // The currently configured maximum speed.
    float Acceleration;     // The currently configured acceleration/deceleration.

    String toJsonString();  // Get a serialized JSON representation.
    String toString();      // Get a string representation.
};
