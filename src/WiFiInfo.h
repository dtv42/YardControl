// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiInfo.h" company="DTV-Online">
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
/// This class holds the actual WiFi connection data.
/// </summary>
class WiFiInfo
{
private:
    StaticJsonDocument<384> _doc; // The Json document representing the data.

public:
    WiFiInfo();

	String Version;							// The WiFi firmware version.
	String Mode;							// The WiFi mode (AP, STA, OFF).
	String SSID;							// The WiFi SSID.
	String Hostname;						// The WiFi hostname.
	String NetworkID;						// The WiFi network ID.
	String Address;							// The WiFi IP address.
	String Gateway;							// The Gateway address.
	String Subnet;							// The Subnet mask.
	String DNS;								// The domain name server.
	String MAC;								// The MAC address.
	int Clients;							// The number of connected clients (AP Mode). 
	long RSSI;								// The signal strength.

    String toJsonString();                  // Get a serialized JSON representation.
    String toString();						// Get a string representation.
};
