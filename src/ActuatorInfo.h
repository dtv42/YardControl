// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ActuatorInfo.h" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Saturday, 18th March 2023 07:14:43</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Saturday, 8th April 2023 07:19:27 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

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
    bool  IsStopped;        // Flag indicating that the motor has been stopped.
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
