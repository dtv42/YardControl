// --------------------------------------------------------------------------------------------------------------------
// <copyright file="YardControl.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:44 PM</created>
// <modified>8-5-2023 8:31 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#if !(defined(ARDUINO_RASPBERRY_PI_PICO_W))
#error For RASPBERRY_PI_PICO_W only
#endif

// Disable all traces when set to 0.
#define ARDUINOTRACE_ENABLE 0

#pragma region Includes

#include <ArduinoTrace.h>

#include <LittleFS.h>
#include <WebServer.h>
#include <Blinkenlight.h>
#include <InputDebounce.h>
#include <TimerInterrupt_Generic.h>

#include "src/AppSettings.h"
#include "src/PicoPins.h"
#include "src/WiFiInfo.h"
#include "src/SystemInfo.h"
#include "src/ServerInfo.h"
#include "src/TelnetServer.h"
#include "src/GpioInputs.h"
#include "src/Wireless.h"
#include "src/Commands.h"
#include "src/Actuator.h"

#pragma endregion

#pragma region Globals

// Message lines to be printed on the serial line during startup.
auto HEADER = "Yard Control - a fiddle yard controller using a linear actuator.";
auto COPYRIGHT = "Copyright (c) 2023 - Dr. Peter Trimmel";

// The global (onboard) Led.
Blinkenlight Led(LED_BUILTIN);

// Create the (global) application settings.
AppSettings Settings;

// The global input and output pins.
GpioPins Pins;

// Create all (debounced) GPIO inputs.
GpioInputs Inputs;

// Create the (global) actuator.
LinearActuator Actuator;

// Create the (global) web server instance (defaults port 80).
WebServer HttpServer(80);

// Create the (global) telnet instance.
TelnetServer Telnet;

// Create the (global) commands instance.
CommandsClass Commands;

// Create the (global) stepper driver timer.
RPI_PICO_Timer Timer(0);

#pragma endregion

#pragma region Timer Callback

bool TimerHandler(struct repeating_timer* t)
{
    (void)t;
    Actuator.onTimer();
    return true;
}

#pragma endregion

#pragma region Telnet Events

/// <summary>
/// Telnet connect event. Print header and prompt.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetConnect(String ip)
{
    TRACE(); DUMP(ip);

    Telnet.println(HEADER);
    Telnet.println(COPYRIGHT);
    Telnet.println();
    Telnet.println("Welcome " + Telnet.getIP() + "");
    Telnet.print(Settings.Telnet.Prompt);
}

/// <summary>
/// Telnet disconnect event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetDisconnect(String ip)
{
    TRACE(); DUMP(ip);
}

/// <summary>
/// Telnet reconnect event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetReconnect(String ip)
{
    TRACE(); DUMP(ip);
}

/// <summary>
/// Telnet connection attempt event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetConnectionAttempt(String ip)
{
    TRACE(); DUMP(ip);
}

/// <summary>
/// Telnet input event. Process the input string and print the prompt.
/// </summary>
/// <param name="str">The input string.</param>
void onTelnetInput(String str)
{
    Commands.parse(str);
    Telnet.print(Settings.Telnet.Prompt);
}

#pragma endregion

#pragma region Setup

/// <summary>
/// Arduino setup function. This is only called once during startup.
/// </summary>
void setup()
{
    // Indicate setup start.
    Led.on();

#pragma region Initialize Serial

    // Initialize serial - default baudrate is 115200 (and wait for port to open if on USB).
    Serial.begin();
    // while (!Serial) { delay(10); }
    delay(1000);

    Serial.println(HEADER);
    Serial.println(COPYRIGHT);
    Serial.println();

#pragma endregion

#pragma region Initialize Settings

    // Initialize the file system and the application settings.
    LittleFS.begin();
    Settings.init();

    // Print system info.
    SystemInfo systemInfo;
    Serial.print(systemInfo.toString());

    // Print application settings.
    Serial.print(Settings.toString());

#pragma endregion

#pragma region Initialize GPIO

    Pins.add(Settings.Stepper.PinPUL, OUTPUT, "PUL");
    Pins.add(Settings.Stepper.PinDIR, OUTPUT, "DIR");
    Pins.add(Settings.Stepper.PinENA, OUTPUT, "ENA");

    Pins.add(Settings.Stepper.PinALM, INPUT_PULLUP, "ALM");

    Pins.add(Settings.Actuator.LedRunning, OUTPUT, "Running");
    Pins.add(Settings.Actuator.LedInLimit, OUTPUT, "Limit");
    Pins.add(Settings.Actuator.LedAlarmOn, OUTPUT, "Alarm");

    Pins.add(Settings.Actuator.SwitchStop,   INPUT_PULLUP, "Stop");
    Pins.add(Settings.Actuator.SwitchLimit1, INPUT_PULLUP, "Limit1");
    Pins.add(Settings.Actuator.SwitchLimit2, INPUT_PULLUP, "Limit2");

    Serial.print(Pins.toString());

    // Initialize debounced inputs.
    Inputs.init();

#pragma endregion

#pragma region Initialize Actuator

    Actuator.init();
    Serial.print(Actuator.toString());

#pragma endregion

#pragma region Initialize WiFi

    // Check for the WiFi module
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");

        // Don't continue, just indicate the error.
        Led.pattern(1, 3);

        while (true)
            Led.update();
    }

    Wireless wireless;
    bool wifiOK = false;

    // Try to connect to WiFi network and create WiFi access point.
    Serial.print("Trying to connect WiFi to '");
    Serial.print(Settings.WiFi.SSID);
    Serial.println("'");

    if (wireless.connectWiFi())
    {
        Serial.println("WiFi connection successful");
        wifiOK = true;
    }
    else
    {
        Serial.println("WiFi connection not successful");
        Serial.println("Trying to create WiFi accesss point");

        if (wireless.createAP())
        {
            Serial.println("Creating a WiFi access point successful");
            wifiOK = true;
        }
        else
        {
            Serial.println("Creating a WiFi access point not successful");
        }
    }

    if (wifiOK)
    {
        // Set the clock using NTP.
        wireless.setClock();

        // Show WiFi info.
        WiFiInfo wifiInfo;
        Serial.print(wifiInfo.toString());
    }
    else
    {
        // Don't continue, just indicate the error.
        Led.pattern(2, 3);

        while (true)
            Led.update();
    }

#pragma endregion

#pragma region Initialize Http

    // Web server setup - GET file
    HttpServer.on("/", getRoot);
    HttpServer.on("/about.html",                  getFile);
    HttpServer.on("/index.html",                  getFile);
    HttpServer.on("/info.html",                   getFile);
    HttpServer.on("/settings.html",               getFile);
    HttpServer.on("/favicon.ico",                 getFile);
    HttpServer.on("/css/custom-styles.css",       getFile);
    HttpServer.on("/css/dark-mode-switch.css",    getFile);
    HttpServer.on("/js/dark-mode-switch.min.js",  getFile);

    // Download the application settings.
    HttpServer.on("/appsettings.json", getAppSettings);

    // Web server setup - GET commands
    HttpServer.on("/settings", getInfo);
    HttpServer.on("/system",   getInfo);
    HttpServer.on("/server",   getInfo);
    HttpServer.on("/status",   getInfo);
    HttpServer.on("/wifi",     getInfo);
    HttpServer.on("/gpio",     getInfo);

    // Web server setup - POST commands
    HttpServer.on("/plus",      postBaseCommand);
    HttpServer.on("/minus",     postBaseCommand);
    HttpServer.on("/forward",   postBaseCommand);
    HttpServer.on("/backward",  postBaseCommand);
    HttpServer.on("/calibrate", postBaseCommand);
    HttpServer.on("/enable",    postBaseCommand);
    HttpServer.on("/disable",   postBaseCommand);
    HttpServer.on("/home",      postBaseCommand);
    HttpServer.on("/stop",      postBaseCommand);
    HttpServer.on("/ramp",      postBaseCommand);
    HttpServer.on("/noramp",    postBaseCommand);
    HttpServer.on("/release",   postBaseCommand);
    HttpServer.on("/reboot",    postReboot);

    // Web server setup - PUT commands with argument
    HttpServer.on("/step",   putIntegerCommand);
    HttpServer.on("/move",   putFloatCommand);
    HttpServer.on("/stepto", putIntegerCommand);
    HttpServer.on("/moveto", putFloatCommand);
    HttpServer.on("/track",  putIntegerCommand);

    // Upload the application settings.
    HttpServer.on("/appsettings.json", postSettings);

    HttpServer.onNotFound(notFound);

    HttpServer.begin(Settings.Http.Port);

#pragma endregion

#pragma region Initialize Telnet

    // Passing on functions for various Telnet events.
    Telnet.onConnect(onTelnetConnect);
    Telnet.onConnectionAttempt(onTelnetConnectionAttempt);
    Telnet.onReconnect(onTelnetReconnect);
    Telnet.onDisconnect(onTelnetDisconnect);
    Telnet.onInputReceived(onTelnetInput);

    if (Telnet.begin(Settings.Telnet.Port))
    {
        // Show TCP server info.
        ServerInfo serverInfo;
        Serial.print(serverInfo.toString());
    }
    else
    {
        Serial.println("Setup failed!");

        // Don't continue, just indicate the error.
        Led.pattern(3, 3);

        while (true)
            Led.update();
    }

#pragma endregion

#pragma region Initialize Timer

    // Initialize timer with maximum frequency (100 kHz).
    if (Timer.setFrequency(Actuator.FREQUENCY, TimerHandler))
    {
        Serial.println("Starting  Timer OK");
    }
    else
    {
        Serial.println("Can't set Timer.");

        // Don't continue, just indicate the error.
        Led.pattern(1, 5);

        while (true)
            Led.update();
    }

#pragma endregion

    // Indicate setup end.
    Led.pattern(1, 1);
}

#pragma endregion

#pragma region Loop

/// <summary>
/// Arduino loop function. Gets called continuosly to update input, output, and other operations.
/// </summary>
void loop()
{
    Led.update();
    Inputs.run();
    Telnet.loop();
    HttpServer.handleClient();
}

#pragma endregion