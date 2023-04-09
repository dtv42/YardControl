// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ServerInfo.h" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Wednesday, 15th March 2023 19:36:33</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Tuesday, 4th April 2023 09:44:06 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <Arduino.h>

#define ARDIUNOJSON_TAB "    "

#include <ArduinoJson.h>

/// <summary>
/// This class holds the actual TCP server settings data.
/// </summary>
class ServerInfo
{
private:
    StaticJsonDocument<192> _doc; // The Json document representing the data.

public:
    ServerInfo();

	String Address;			// The TCP server IP address on the WiFi.
	String Name;			// The TCP server hostname.
	String Mode;			// The WiFi mode (AP, STA).
	int    Port;		    // The Web server IP port.
    int    Telnet;          // The telnet server IP port.
    String Prompt;			// The Server prompt.

    String toJsonString();  // Get a serialized JSON representation.
    String toString();		// Get a string representation.
};
