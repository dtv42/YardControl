// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ServerInfo.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
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
