// --------------------------------------------------------------------------------------------------------------------
// <copyright file="AppSettings.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>26-4-2023 8:17 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <StringSplitter.h>

#include "AppSettings.h"

/// <summary>
/// Used for printing error messages when reading or writing the settings file.
/// </summary>
extern SerialUSB Serial; 

/// <summary>
/// Tabify the string (breaking on LF) by adding four spaces.
/// </summary>
/// <param name="text">The text containing lines to be indented.</param>
/// <returns>The indented text.</returns>
String AppSettings::_addTab(String text)
{
    StringSplitter *splitter = new StringSplitter(text, '\n', AppSettings::MAX_LINES);
    int itemCount = splitter->getItemCount();
    String result;

    for (int i = 0; i < itemCount; i++)
    {
        result += String("    ") + splitter->getItemAtIndex(i) + String('\n');
    }

    return result;
}

/// <summary>
/// Updates the static JSON document from the current settings.
/// </summary>
void AppSettings::_update()
{
    _doc.clear();

    _doc["Yard"]     = Yard.toJson();
    _doc["Actuator"] = Actuator.toJson();
    _doc["Stepper"]  = Stepper.toJson();
    _doc["Http"]     = Http.toJson();
    _doc["Telnet"]   = Telnet.toJson();
    _doc["WiFi"]     = WiFi.toJson();
    _doc["AP"]       = AP.toJson();
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::YardSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        JsonArray tracks = json["Tracks"];

        if ((tracks != nullptr) && (tracks.size() == YardSettings::MAX_TRACKS))
        {
            for (int i = 0; i < tracks.size(); i++)
            {
                Tracks[i] = tracks[i];
            }
        }
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::YardSettings::toJson()
{
    _doc.clear();

    JsonArray tracks = _doc.createNestedArray("Tracks");
    tracks.add(Tracks[0]);
    tracks.add(Tracks[1]);
    tracks.add(Tracks[2]);
    tracks.add(Tracks[3]);
    tracks.add(Tracks[4]);
    tracks.add(Tracks[5]);
    tracks.add(Tracks[6]);
    tracks.add(Tracks[7]);
    tracks.add(Tracks[8]);
    tracks.add(Tracks[9]);

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::YardSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::YardSettings::toString()
{
    return String("Yard:") + "\r\n" +
                  "    Track 0: " + Tracks[0] + "\r\n" +
                  "    Track 1: " + Tracks[1] + "\r\n" +
                  "    Track 2: " + Tracks[2] + "\r\n" +
                  "    Track 3: " + Tracks[3] + "\r\n" +
                  "    Track 4: " + Tracks[4] + "\r\n" +
                  "    Track 5: " + Tracks[5] + "\r\n" +
                  "    Track 6: " + Tracks[6] + "\r\n" +
                  "    Track 7: " + Tracks[7] + "\r\n" +
                  "    Track 8: " + Tracks[8] + "\r\n" +
                  "    Track 9: " + Tracks[9] + "\r\n";
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::ActuatorSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        SwitchLimit1 = json["SwitchLimit1"] | SwitchLimit1;
        SwitchLimit2 = json["SwitchLimit2"] | SwitchLimit2;
        SwitchStop   = json["SwitchStop"]   | SwitchStop;

        LedRunning = json["LedRunning"] | LedRunning;
        LedInLimit = json["LedInLimit"] | LedInLimit;
        LedAlarmOn = json["LedAlarmOn"] | LedAlarmOn;

        MoveSpeed = json["MoveSpeed"] | MoveSpeed;
        Retract   = json["Retract"]   | Retract;
        Length    = json["Length"]    | Length;
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::ActuatorSettings::toJson()
{
    _doc.clear();
    _doc["SwitchLimit1"] = SwitchLimit1;
    _doc["SwitchLimit2"] = SwitchLimit2;
    _doc["SwitchStop"]   = SwitchStop;

    _doc["LedRunning"] = LedRunning;
    _doc["LedInLimit"] = LedInLimit;
    _doc["LedAlarmOn"] = LedAlarmOn;

    _doc["MoveSpeed"] = MoveSpeed;
    _doc["Retract"]   = Retract;
    _doc["Length"]    = Length;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::ActuatorSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::ActuatorSettings::toString()
{
    return String("Actuator:") + "\r\n" +
                  "    SwitchLimit1: " + SwitchLimit1 + "\r\n" +
                  "    SwitchLimit1: " + SwitchLimit2 + "\r\n" +
                  "    SwitchStop:   " + SwitchStop   + "\r\n" +
                  "    LedRunning:   " + LedRunning   + "\r\n" +
                  "    LedInLimit:   " + LedInLimit   + "\r\n" +
                  "    LedAlarmOn:   " + LedAlarmOn   + "\r\n" +
                  "    MoveSpeed:    " + MoveSpeed    + "\r\n" +
                  "    Retract:      " + Retract      + "\r\n" +
                  "    Length:       " + Length       + "\r\n";
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::StepperSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        PinPUL              = json["PinPUL"]              | PinPUL;
        PinDIR              = json["PinDIR"]              | PinDIR;
        PinENA              = json["PinENA"]              | PinENA;
        PinALM              = json["PinALM"]              | PinALM;
        MicroSteps          = json["MicroSteps"]          | MicroSteps;
        MaxSpeed            = json["MaxSpeed"]            | MaxSpeed;
        Acceleration        = json["Acceleration"]        | Acceleration;
        StepsPerRotation    = json["StepsPerRotation"]    | StepsPerRotation;
        DistancePerRotation = json["DistancePerRotation"] | DistancePerRotation;
        MinPulseWidth       = json["MinPulseWidth"]       | MinPulseWidth;
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::StepperSettings::toJson()
{
    _doc.clear();
    _doc["PinPUL"]              = PinPUL;
    _doc["PinDIR"]              = PinDIR;
    _doc["PinENA"]              = PinENA;
    _doc["PinALM"]              = PinALM;
    _doc["MicroSteps"]          = MicroSteps; 
    _doc["MaxSpeed"]            = MaxSpeed; 
    _doc["Acceleration"]        = Acceleration; 
    _doc["StepsPerRotation"]    = StepsPerRotation;
    _doc["DistancePerRotation"] = DistancePerRotation;
    _doc["MinPulseWidth"]       = MinPulseWidth;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::StepperSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::StepperSettings::toString()
{
    return String("Stepper:") + "\r\n" +
                  "    PinPUL:              " + PinPUL              + "\r\n" +
                  "    PinDIR:              " + PinDIR              + "\r\n" +
                  "    PinENA:              " + PinENA              + "\r\n" +
                  "    PinALM:              " + PinALM              + "\r\n" +
                  "    MicroSteps:          " + MicroSteps          + "\r\n" +
                  "    MaxSpeed:            " + MaxSpeed            + "\r\n" +
                  "    Acceleration:        " + Acceleration        + "\r\n" +
                  "    StepsPerRotation:    " + StepsPerRotation    + "\r\n" +
                  "    DistancePerRotation: " + DistancePerRotation + "\r\n" +
                  "    MinPulseWidth:       " + MinPulseWidth       + "\r\n";
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::HttpSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        Port = json["Port"] | Port;
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::HttpSettings::toJson()
{
    _doc.clear();
    _doc["Port"] = Port;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::HttpSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::HttpSettings::toString()
{
    return String("Http:") + "\r\n" +
                  "    Port:   " + Port + "\r\n";
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::TelnetSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        Port   = json["Port"]   | Port;
        Prompt = json["Prompt"] | Prompt;
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::TelnetSettings::toJson()
{
    _doc.clear();
    _doc["Port"]   = Port;
    _doc["Prompt"] = Prompt;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::TelnetSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::TelnetSettings::toString()
{
    return String("Telnet:") + "\r\n" +
                  "    Port:   " + Port + "\r\n" +
                  "    Prompt: " + Prompt + "\r\n";
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::WiFiSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        DHCP     = json["DHCP"]     | DHCP;
        SSID     = json["SSID"]     | SSID;
        Password = json["Password"] | Password;
        Hostname = json["Hostname"] | Hostname;
        Address  = json["Address"]  | Address;
        Gateway  = json["Gateway"]  | Gateway;
        Subnet   = json["Subnet"]   | Subnet;
        DNS      = json["DNS"]      | DNS;
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::WiFiSettings::toJson()
{
    _doc.clear();
    _doc["DHCP"]     = DHCP;
    _doc["SSID"]     = SSID;
    _doc["Password"] = Password;
    _doc["Hostname"] = Hostname;
    _doc["Address"]  = Address;
    _doc["Gateway"]  = Gateway;
    _doc["Subnet"]   = Subnet;
    _doc["DNS"]      = DNS;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::WiFiSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::WiFiSettings::toString()
{
    return String("WiFi:") + "\r\n" +
                  "    DHCP:     " + ((DHCP == 1) ? "true" : "false") + "\r\n" +
                  "    SSID:     " + SSID     + "\r\n" +
                  "    Password: " + Password + "\r\n" +
                  "    Hostname: " + Hostname + "\r\n" +
                  "    Address:  " + Address  + "\r\n" +
                  "    Gateway:  " + Gateway  + "\r\n" + 
                  "    Subnet:   " + Subnet   + "\r\n" +
                  "    DNS:      " + DNS      + "\r\n";
}

/// <summary>
/// Update data fields from JSON representation.
/// </summary>
/// <param name="json">The JSON object containing the settings.</param>
void AppSettings::APSettings::fromJson(JsonObject json)
{
    if (json != nullptr)
    {
        SSID     = json["SSID"]     | SSID;
        Password = json["Password"] | Password;
        Hostname = json["Hostname"] | Hostname;
    }
}

/// <summary>
/// Creates a JSON object with the current settings.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject AppSettings::APSettings::toJson()
{
    _doc.clear();
    _doc["SSID"]     = SSID;
    _doc["Password"] = Password;
    _doc["Hostname"] = Hostname;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a (pretty) string representation of the current settings.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::APSettings::toJsonString()
{
    String json;
    serializeJsonPretty(toJson(), json);
    return json;
}

/// <summary>
/// Creates a printable string representation of the settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::APSettings::toString()
{
    return String("AP :") + "\r\n" +
                    "    SSID:     " + SSID     + "\r\n" +
                    "    Password: " + Password + "\r\n" +
                    "    Hostname: " + Hostname + "\r\n";
}

/// <summary>
/// Initializes all application settings reading from the 'appsettings.json' file.
/// The default values are preserved if the particular setting is not found.
/// </summary>
/// <returns>True if successful.</returns>
bool AppSettings::init()
{
    File file = LittleFS.open(SETTINGS_FILE, "r");
    DeserializationError error = deserializeJson(_doc, file);
    file.close();

    if (error)
    {
        Serial.print(String("Deserialize settings from ") + SETTINGS_FILE + " failed: ");
        Serial.println(error.f_str());
        return false;
    }
    else
    {
        Yard.fromJson(_doc["Yard"]);
        Actuator.fromJson(_doc["Actuator"]);
        Stepper.fromJson(_doc["Stepper"]);
        Http.fromJson(_doc["Http"]);
        Telnet.fromJson(_doc["Telnet"]);
        WiFi.fromJson(_doc["WiFi"]);
        AP.fromJson(_doc["AP"]);
    }

    return true;
}

/// <summary>
/// Saves the current settings (JSON document) in the 'appsettings.json' file.
/// </summary>
/// <returns>True if successful.</returns>
bool AppSettings::save()
{
    _update();

    File file = LittleFS.open(SETTINGS_FILE, "w");
    size_t size = serializeJsonPretty(_doc, file);
    file.close();

    if (size == 0) {
        Serial.print(String("Serialize  to ") + SETTINGS_FILE + " failed.");
        return false;
    }

    return true;
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String AppSettings::toJsonString()
{
    _update();
    
    String json;
    serializeJsonPretty(_doc, json);
    return json;
}

/// <summary>
/// Returns a printable string representation of the application settings.
/// </summary>
/// <returns>The printable string.</returns>
String AppSettings::toString()
{
    _update();

    return String("Application ") +
           _addTab(Yard.toString()) +
           _addTab(Actuator.toString()) +
           _addTab(Stepper.toString()) +
           _addTab(Http.toString()) +
           _addTab(Telnet.toString()) +
           _addTab(WiFi.toString()) +
           _addTab(AP.toString());
}