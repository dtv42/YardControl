﻿// --------------------------------------------------------------------------------------------------------------------
// <copyright file="AppSettings.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>10-4-2023 11:38 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <array>

#include <LittleFS.h>

#define SETTINGS_FILE "appsettings.json"
#define ARDIUNOJSON_TAB "    "

#include <ArduinoJson.h>

class AppSettings
{
private:
    static const int MAX_LINES = 12;            // The maximum number of lines in tabbed printout.
    StaticJsonDocument<1536> _doc;              // The Json document representing the application settings.

    String _addTab(String text);                // Tabify the string (breaking on LF).
    void _update();                             // Updates the Json document.

public:
    class YardSettings
    {
    private:
        StaticJsonDocument<192> _doc;           // The Json document representing the settings.

    public:
        static const int MAX_TRACKS = 10;       // The number of supported tracks.

        std::array<long, YardSettings::MAX_TRACKS> Tracks = {
            1600 * 0,                           // default track position track 0.
            1600 * 33,                          // default track position track 1.
            1600 * 66,                          // default track position track 2.
            1600 * 99,                          // default track position track 3.
            1600 * 132,                         // default track position track 4.
            1600 * 165,                         // default track position track 5.
            1600 * 198,                         // default track position track 6.
            1600 * 231,                         // default track position track 7.
            1600 * 264,                         // default track position track 8.
            1600 * 297                          // default track position track 9.
        };

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString();                      // Get a string representation.
    };

    class ActuatorSettings
    {
    private:
        StaticJsonDocument<128> _doc;           // The Json document representing the settings.

    public:
        uint8_t  SwitchStop   = 6;              // The emergency stop input pin number.
        uint8_t  SwitchLimit1 = 7;              // The limit 1 input pin number (limit1).
        uint8_t  SwitchLimit2 = 8;              // The limit 2 input pin number (limit2).

        uint8_t  LedRunning   = 3;              // Led output pin number (is running).
        uint8_t  LedLimit     = 4;              // Led output pin number (limit 1 hit).
        uint8_t  LedStop      = 5;              // Led output pin number (limit 2 hit).

        float    MoveSpeed    = 100;            // Calibration speed to move toward limit.
        uint16_t Retract      = 1600;           // Retract distance when limit is hit (steps).

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString();                      // Get a string representation.
    };

    class StepperSettings
    {
    private:
        StaticJsonDocument<192> _doc; // The Json document representing the settings.

    public:
        uint8_t  PinPUL              = 0;       // The output pin number for driver PUL input (step).
        uint8_t  PinDIR              = 1;       // The output pin number for driver DIR input (direction).
        uint8_t  PinENA              = 2;       // The output pin number for driver ENA input (enable).
        uint8_t  MicroSteps          = 1;       // The multiplication factor for steps (max. 32).
        float    MaxSpeed            = 1000.0;  // The desired maximum speed in steps per second. Must be > 0.
        float    Acceleration        = 50.0;    // The acceleration in steps per second per second. Must be > 0.0.
        uint16_t StepsPerRotation    = 200;     // The number of steps per rotation (360°).
        float    DistancePerRotation = 1.0;     // The distance in mm per rotation (360°).
        uint16_t MinPulseWidth       = 20;      // The minimum allowed pulse width.

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString(); 	                    // Get a string representation.
    };

    class HttpSettings
    {
    private:
        StaticJsonDocument<16> _doc;            // The Json document representing the settings.

    public:
        uint16_t Port = 80;                     // The Http Server port number.

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString();                      // Get a string representation.
    };

    class TelnetSettings
    {
    private:
        StaticJsonDocument<48> _doc;            // The Json document representing the settings.

    public:
        uint16_t Port   = 23;                   // The Telnet Server port number.
        String   Prompt = ">";                  // The command line input prompt.

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString();                      // Get a string representation.
    };

    class WiFiSettings
    {
    private:
        StaticJsonDocument<384> _doc;           // The Json document representing the settings.

    public:
        bool   DHCP = true;		                // The WiFi DHCP mode.
        String SSID;		                    // The WiFi SSID.
        String Password;	                    // The WiFi Passphrase.
        String Hostname;	                    // The WiFi Hostname.
        String Address;                         // The static Address.
        String Gateway;		                    // The Gateway address.
        String Subnet;		                    // The SubnetMask.
        String DNS;			                    // The domain name server.

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString();                      // Get a string representation.
    };

    class APSettings
    {
    private:
        StaticJsonDocument<192> _doc; // The Json document representing the settings.

    public:
        String SSID;	                        // The WiFi Access Point SSID.
        String Password;                        // The WiFi Access Point passphrase.
        String Hostname;                        // The WiFi Access Point hostname

        void fromJson(JsonObject json);         // Update from JSON representation.
        JsonObject toJson();                    // Get a JSON representation.
        String toString();                      // Get a string representation.
    };

    YardSettings     Yard;
    ActuatorSettings Actuator;
    StepperSettings  Stepper;
    HttpSettings     Http;
    TelnetSettings   Telnet;
    WiFiSettings     WiFi;
    APSettings       AP;

    bool init();            // Initializes the settings from the appsettings.json file.
    bool save();            // Updates and saves the settings to the appsettings.json file.

    String toJsonString(); 	// Get a serialized JSON representation.
    String toString(); 		// Get a string representation.
};