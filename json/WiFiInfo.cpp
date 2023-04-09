// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiInfo.cpp" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Wednesday, 15th March 2023 19:36:33</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Tuesday, 4th April 2023 08:23:05 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <WiFi.h>

#include "WiFiInfo.h"

#define CRLF "\r\n"

/// <summary>
///  Using a WiFi instance to get the actual data.
/// </summary>
WiFiInfo::WiFiInfo()
{
	Version  = String(WiFi.firmwareVersion());
	Mode     = String("Unknown");
	SSID     = String("Unknown");
	Hostname = String("Unknown");
	Address  = String("Unknown");
	Gateway  = String("Unknown");
	Subnet   = String("Unknown");
	MAC      = String("Unknown");
	Clients  = 0;
	RSSI     = 0L;

	Gateway = WiFi.gatewayIP().toString();
	Subnet = WiFi.subnetMask().toString();
    MAC = WiFi.macAddress();
    MAC.toUpperCase();

	WiFiMode_t mode = WiFi.getMode();

	if (mode == WIFI_AP)
	{
		Mode = String("AP");
		SSID = WiFi.softAPSSID();
		Hostname = String(WiFi.getHostname());
		Address = WiFi.softAPIP().toString();
		Clients = WiFi.softAPgetStationNum();
	}
	else if (mode == WIFI_STA)
	{
		Mode = String("STA");
		SSID = WiFi.SSID();
		Hostname = String(WiFi.getHostname());
		Address = WiFi.localIP().toString();
	    RSSI = WiFi.RSSI();
	}
	else if (mode == WIFI_OFF)
	{
		Mode = String("OFF");
	}	
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String WiFiInfo::toJsonString()
{
    _doc.clear();

    if (Mode == "AP")
	{
        String json;
      
        _doc["Version"]  = Version;
        _doc["Mode"]     = Mode;
        _doc["SSID"]     = SSID;
        _doc["Hostname"] = Hostname;
        _doc["Address"]  = Address;
        _doc["Gateway"]  = Gateway;
        _doc["Subnet"]   = Subnet;
        _doc["MAC"]      = MAC;
        _doc["Clients"]  = Clients;
        serializeJsonPretty(_doc, json);

        return json;
	}
    else if (Mode == "STA")
    {
        String json;
        DynamicJsonDocument doc(300);

        _doc["Version"]  = Version;
        _doc["Mode"]     = Mode;
        _doc["SSID"]     = SSID;
        _doc["Hostname"] = Hostname;
        _doc["Address"]  = Address;
        _doc["Gateway"]  = Gateway;
        _doc["Subnet"]   = Subnet;
        _doc["DNS"]      = DNS;
        _doc["RSSI"]     = RSSI;
        _doc["MAC"]      = MAC;
        serializeJsonPretty(_doc, json);

        return json;
    }
    else {
        return String("{}") + CRLF;
    }
}

/// <summary>
/// Returns a printable string representation of the system info.
/// </summary>
/// <returns>The printable string.</returns>
String WiFiInfo::toString()
{
	if (Mode == "AP")
	{
        return String("WiFi AP Info:") + CRLF +
                      "    Version:  " + Version  + CRLF +
                      "    Mode:     " + Mode     + CRLF +
                      "    SSID:     " + SSID     + CRLF +
                      "    Hostname: " + Hostname + CRLF +
                      "    Address:  " + Address  + CRLF +
                      "    Gateway:  " + Gateway  + CRLF +
                      "    Subnet:   " + Subnet   + CRLF +
                      "    MAC:      " + MAC      + CRLF +
                      "    Clients:  " + Clients  + CRLF +
                      CRLF;
	}
    else if (Mode == "STA")
    {
        return String("WiFi Connection Info:") + CRLF +
                      "    Version:  " + Version  + CRLF +
                      "    Mode:     " + Mode     + CRLF +
                      "    SSID:     " + SSID     + CRLF +
                      "    Hostname: " + Hostname + CRLF +
                      "    Address:  " + Address  + CRLF +
                      "    Gateway:  " + Gateway  + CRLF +
                      "    Subnet:   " + Subnet   + CRLF +
                      "    DNS:      " + DNS      + CRLF +
                      "    RSSI:     " + RSSI     + CRLF +
                      "    MAC:      " + MAC      + CRLF +
                      CRLF;
    }
    else 
    {
        return String("WiFi Info: N/A");
    }
}