// --------------------------------------------------------------------------------------------------------------------
// <copyright file="SystemInfo.cpp" company="DTV-Online">
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

#include "SystemInfo.h"
#include "Version.h"

#define CRLF "\r\n"

/// <summary>
///  Using the global RP2040 instance to get the actual data.
/// </summary>
SystemInfo::SystemInfo()
{
    time_t now = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    String time = asctime(&timeinfo);
    SystemTime = time.substring(0, time.length() - 2);
    Software = String(Auto::SOFTWARE_VERSION) + " " + Auto::LAST_MODIFIED_DATE;
    BoardInfo = String("Raspberry Pi Pico W");
    BoardID    = String(rp2040.getChipID());
    BoardTemp  = analogReadTemp();
	CpuFreqMHz = rp2040.f_cpu() / 1000000;
	HeapSize   = rp2040.getTotalHeap() / 1000;
	FreeHeap   = rp2040.getFreeHeap() / 1000;
	UsedHeap   = rp2040.getUsedHeap() / 1000;
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String SystemInfo::toJsonString()
{
    String json;

    _doc.clear();
    _doc["SystemTime"] = SystemTime;
    _doc["Software"]   = Software;
    _doc["BoardInfo"]  = BoardInfo;
    _doc["BoardID"]    = BoardID;
    _doc["BoardTemp"]  = BoardTemp;
    _doc["CpuFreqMHz"] = CpuFreqMHz;
    _doc["HeapSize"]   = HeapSize;
    _doc["FreeHeap"]   = FreeHeap;
    _doc["UsedHeap"]   = UsedHeap;
    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Returns a printable string representation of the system info.
/// </summary>
/// <returns>The printable string.</returns>
String SystemInfo::toString()
{
	return String("System Info:") + CRLF +
	              "    SystemTime: " + SystemTime + CRLF +
	              "    Software:   " + Software   + CRLF +
	              "    BoardInfo:  " + BoardInfo  + CRLF +
	              "    BoardID:    " + BoardID    + CRLF +
	              "    BoardTemp:  " + BoardTemp  + CRLF +
	              "    CpuFreqMHz: " + CpuFreqMHz + CRLF +
	              "    HeapSize:   " + HeapSize   + CRLF +
	              "    FreeHeap:   " + FreeHeap   + CRLF +
	              "    UsedHeap:   " + UsedHeap   + CRLF +
                  CRLF;
}