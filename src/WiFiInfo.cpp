// --------------------------------------------------------------------------------------------------------------------
// <copyright file="WiFiInfo.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>11-4-2023 9:11 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include <WiFi.h>

#include "WiFiInfo.h"

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
        return String("{}") + "\r\n";
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
        return String("WiFi AP Info:") + "\r\n" +
                      "    Version:  " + Version  + "\r\n" +
                      "    Mode:     " + Mode     + "\r\n" +
                      "    SSID:     " + SSID     + "\r\n" +
                      "    Hostname: " + Hostname + "\r\n" +
                      "    Address:  " + Address  + "\r\n" +
                      "    Gateway:  " + Gateway  + "\r\n" +
                      "    Subnet:   " + Subnet   + "\r\n" +
                      "    MAC:      " + MAC      + "\r\n" +
                      "    Clients:  " + Clients  + "\r\n" +
                      "\r\n";
	}
    else if (Mode == "STA")
    {
        return String("WiFi Connection Info:") + "\r\n" +
                      "    Version:  " + Version  + "\r\n" +
                      "    Mode:     " + Mode     + "\r\n" +
                      "    SSID:     " + SSID     + "\r\n" +
                      "    Hostname: " + Hostname + "\r\n" +
                      "    Address:  " + Address  + "\r\n" +
                      "    Gateway:  " + Gateway  + "\r\n" +
                      "    Subnet:   " + Subnet   + "\r\n" +
                      "    DNS:      " + DNS      + "\r\n" +
                      "    RSSI:     " + RSSI     + "\r\n" +
                      "    MAC:      " + MAC      + "\r\n" +
                      "\r\n";
    }
    else 
    {
        return String("WiFi Info: N/A");
    }
}