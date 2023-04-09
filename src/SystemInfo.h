// --------------------------------------------------------------------------------------------------------------------
// <copyright file="SystemInfo.h" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Wednesday, 15th March 2023 19:36:33</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Friday, 7th April 2023 10:45:12 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <Arduino.h>

#define ARDIUNOJSON_TAB "    "

#include <ArduinoJson.h>

#include "Version.h"

/// <summary>
/// This class holds the current system data.
/// </summary>
class SystemInfo
{
private:
    StaticJsonDocument<384> _doc; // The Json document representing the data.

public:
	SystemInfo();							// Initialize the system info fields

    String SystemTime;                      // The current system time.
    String Software;                        // Software version and date.
    String BoardInfo;                       // The board information (Pico W).
    String BoardID;                         // Board identifier (MAC address).
    float  BoardTemp;                       // Board temperature (°C).
    int CpuFreqMHz;                         // The CPU frequency in MHz.
    int HeapSize;							// The total heap size in kB.
	int FreeHeap;							// The amount of free heap kB.
	int UsedHeap;							// The amount of used heap kB.

    String toJsonString();                  // Get a serialized JSON representation.
    String toString();						// Get a string representation.
};
