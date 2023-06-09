// --------------------------------------------------------------------------------------------------------------------
// <copyright file="ServerInfo.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>9-5-2023 9:08 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <WiFi.h>

#include "AppSettings.h"
#include "ServerInfo.h"

extern AppSettings Settings;

/// <summary>
///  Using a WiFi instance to get the actual data.
/// </summary>
ServerInfo::ServerInfo()
{
	WiFiMode_t mode = WiFi.getMode();

	if (mode == WiFiMode_t::WIFI_AP)
	{
		Mode = String("AP");
		Address = WiFi.softAPIP().toString();
	}
	else if (mode == WiFiMode_t::WIFI_STA)
	{
		Mode = String("STA");
		Address = WiFi.localIP().toString();
	}
	else if (mode == WiFiMode_t::WIFI_OFF)
	{
		Mode = String("OFF");
		Address = String("Unknown");
	}
	else 
	{
		Mode = String("Unknown");
		Address = String("Unknown");
	}

    Name   = String(WiFi.getHostname());
    Port   = Settings.Server.Http;
    Telnet = Settings.Server.Telnet;
    Prompt = Settings.Server.Prompt;
}

/// <summary>
/// Get a JSON representation.
/// </summary>
String ServerInfo::toJsonString()
{
    String json;

    _doc.clear();
    _doc["Address"] = Address;
    _doc["Name"]    = Name;
    _doc["Mode"]    = Mode;
    _doc["Port"]    = Port;
    _doc["Telnet"]  = Telnet;
    _doc["Prompt"]  = Prompt;
    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Get a string representation.
/// </summary>
String ServerInfo::toString()
{
	return String("TCP Server Info:") + "\r\n" +
	              "    Address: " + Address + "\r\n" +
	              "    Name:    " + Name    + "\r\n" +
	              "    Mode:    " + Mode    + "\r\n" +
	              "    Port:    " + Port    + "\r\n" +
                  "    Telnet:  " + Telnet  + "\r\n" +
	              "    Prompt:  " + Prompt  + "\r\n";
}


