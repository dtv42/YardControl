// --------------------------------------------------------------------------------------------------------------------
// <copyright file="YardControl.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:44 PM</created>
// <modified>21-4-2023 3:42 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma region Includes

#include "src/Defines.h"

#include <LittleFS.h>
#include <WebServer.h>
#include <Blinkenlight.h>
#include <InputDebounce.h>

#include "src/AppSettings.h"
#include "src/PicoPins.h"
#include "src/WiFiInfo.h"
#include "src/SystemInfo.h"
#include "src/ServerInfo.h"
#include "src/TelnetServer.h"
#include "src/ActuatorInfo.h"
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

#pragma endregion

#pragma region Http Helper

/// <summary>
/// Stream a file to the Http client. The Mime type is set based on the file extension. 
/// </summary>
/// <param name="path">The path of the file.</param>
void sendFile(String path)
{
    String mime = "application/octet-stream";

    if (path.endsWith(".html"))
    {
        mime = "text/html";
    }
    else if (path.endsWith(".css"))
    {
        mime = "text/css";
    }
    else if (path.endsWith(".js"))
    {
        mime = "application/javascript";
    }
    else if (path.endsWith(".json"))
    {
        mime = "application/json";
    }
    else if (path.endsWith(".ico"))
    {
        mime = "image/x-icon";
    }

    if (LittleFS.exists(path))
    {
        File file = LittleFS.open(path, "r");

        if (!file)
        {
            HttpServer.send(500, "text/plain", String("Error opening file ") + path + ".");
        }
        else
        {
            HttpServer.setContentLength(file.size());
            HttpServer.streamFile(file, mime);
        }

        file.close();
    }
    else
    {
        HttpServer.send(404, "text/plain", String("File ") + path + " not found");
    }
}

/// <summary>
/// Recieves a file (string content) and writes to the file system.
/// Note this works for small files only.
/// </summary>
/// <param name="path">The path of the file.</param>
/// <param name="content">The file content (string).</param>
void receiveFile(String path, String content)
{
    File file = LittleFS.open(path, "w");

    if (!file)
    {
        HttpServer.send(500, "text/plain", String("Error opening file ") + path + ".");
    }
    else
    {
        if (file.print(content))
        {
            HttpServer.send(200, "text/plain", "OK");
        }
        else
        {
            HttpServer.send(500, "text/plain", String("Saving file to ") + path + " failed.");
        }
    }

    file.close();
}

/// <summary>
/// Return the default file ('index.html').
/// </summary>
void getRoot()
{
    if (HttpServer.method() != HTTP_GET)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        sendFile("/www/index.html");
    }
}

/// <summary>
/// Returns requested file (uri).
/// </summary>
void getFile()
{
    if (HttpServer.method() != HTTP_GET)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        sendFile(String("/www") + HttpServer.uri());
    }
}

/// <summary>
/// Returns the application settings file ('appsettings.json').
/// </summary>
void getAppSettings()
{
    sendFile(HttpServer.uri());
}

/// <summary>
/// Returns a JSON representation of various info items.
/// </summary>
void getInfo()
{
    if (HttpServer.method() != HTTP_GET)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        String json;
        String info = HttpServer.uri().substring(1);

        if (info == "settings")
        {
            json = Settings.toJsonString();
        }
        else if (info == "system")
        {
            SystemInfo systemInfo;
            json = systemInfo.toJsonString();
        }
        else if (info == "server")
        {
            ServerInfo serverInfo;
            json = serverInfo.toJsonString();
        }
        else if (info == "status")
        {
            ActuatorInfo actuatorInfo;
            json = actuatorInfo.toJsonString();
        }
        else if (info == "wifi")
        {
            WiFiInfo wifiInfo;
            json = wifiInfo.toJsonString();
        }
        else if (info == "gpio")
        {
            json = Pins.toJsonString();
        }
        else
        {
            HttpServer.send(500, "text/plain", "Could not find the info");
            return;
        }

        HttpServer.send(200, "application/json", json);
    }
}

/// <summary>
/// Execute basic command (no arguments).
/// </summary>
void postBaseCommand()
{
    if (HttpServer.method() != HTTP_POST)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        String command = HttpServer.uri().substring(1);

        if (Commands.isValidBaseCommand(command))
        {
            Commands.parse(command);
            HttpServer.send(200, "text/plain", "OK");
        }
        else
        {
            HttpServer.send(404, "text/plain", String("Command ") + command + " not found");
        }
    }
}

/// <summary>
/// Execute integer command (one integer argument).
/// </summary>
void postIntegerCommand()
{
    if (HttpServer.method() != HTTP_POST)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        int n = HttpServer.args();

        if (n == 0)
        {
            HttpServer.send(400, "text/plain", "A single argument expected");
        }
        if (n > 1)
        {
            HttpServer.send(400, "text/plain", "Only one argument expected");
        }
        else
        {
            String arg = HttpServer.arg(0);

            if (Commands.isInteger(arg))
            {
                String command = HttpServer.uri().substring(1);

                if (Commands.isValidLongCommand(command))
                {
                    command += " " + arg;
                    Commands.parse(command);
                    HttpServer.send(200, "text/plain", "OK");
                }
                else
                {
                    HttpServer.send(404, "text/plain", String("Command ") + command + " not found");
                }
            }
            else
            {
                HttpServer.send(400, "text/plain", String("Argument ") + arg + " not a valid integer");
            }
        }
    }
}

/// <summary>
/// Execute float command (one float argument).
/// </summary>
void postFloatCommand()
{
    if (HttpServer.method() != HTTP_POST)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        int n = HttpServer.args();

        if (n == 0)
        {
            HttpServer.send(400, "text/plain", "A single argument expected");
        }
        if (n > 1)
        {
            HttpServer.send(400, "text/plain", "Only one argument expected");
        }
        else
        {
            String arg = HttpServer.arg(0);

            if (Commands.isFloat(arg))
            {
                String command = HttpServer.uri().substring(1);

                if (Commands.isValidFloatCommand(command))
                {
                    command += " " + arg;
                    Commands.parse(command);
                    HttpServer.send(200, "text/plain", "OK");
                }
                else
                {
                    HttpServer.send(404, "text/plain", String("Command ") + command + " not found");
                }
            }
            else
            {
                HttpServer.send(400, "text/plain", String("Argument ") + arg + " not a valid number");
            }
        }
    }
}

/// <summary>
/// Receive the application settings file ('appsettings.json').
/// </summary>
void postSettings()
{
    int n = HttpServer.args();

    if (n != 1)
    {
        HttpServer.send(400, "text/plain", "A file content (JSON) was expected.");
    }
    else
    {
        String arg = HttpServer.arg(0);
        receiveFile(HttpServer.uri(), arg);
    }
}

/// <summary>
/// Reboot the system.
/// </summary>
void postReboot()
{
    // We don't want to wait for a telnet response - rebooting immediately.
    Telnet.print("Rebooting...");
    Telnet.disconnectClient();
    rp2040.reboot();
}

/// <summary>
/// Not found handler (returns the about page).
/// </summary>
void notFound()
{
    sendFile("/www/about.html");
}

#pragma endregion

#pragma region Telnet Events

/// <summary>
/// Telnet connect event. Print header and prompt.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetConnect(String ip)
{
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
}

/// <summary>
/// Telnet reconnect event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetReconnect(String ip)
{
}

/// <summary>
/// Telnet connection attempt event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetConnectionAttempt(String ip)
{
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

    // Initialize serial and wait for port to open (if on USB).
    Serial.begin(SERIAL_SPEED);
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

#pragma region Initialize Gpio

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

#pragma endregion

#pragma region Initialize Inputs

    Inputs.init();

#pragma endregion

#pragma region Initialize Actuator

    Actuator.init();
    ActuatorInfo actuatorInfo;
    Serial.print(actuatorInfo.toString());

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
    HttpServer.on("/css/custom-input-switch.css", getFile);
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
    HttpServer.on("/release",   postBaseCommand);
    HttpServer.on("/reboot",    postReboot);

    // Web server setup - POST commands with argument
    HttpServer.on("/step",         postIntegerCommand);
    HttpServer.on("/move",         postFloatCommand);
    HttpServer.on("/stepto",       postIntegerCommand);
    HttpServer.on("/moveto",       postFloatCommand);
    HttpServer.on("/track",        postIntegerCommand);
    HttpServer.on("/speed",        postFloatCommand);
    HttpServer.on("/maxspeed",     postFloatCommand);
    HttpServer.on("/acceleration", postFloatCommand);

    // Upload the application settings.
    HttpServer.on("/appsettings.json", postSettings);

    HttpServer.onNotFound(notFound);

    HttpServer.begin(Settings.Http.Port);

#pragma endregion

#pragma region Telnet Server

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

    // Indicate setup end.
    Led.pattern(1, 1);
}

#pragma endregion

#pragma region Loop

/// <summary>
/// Arduino loop function. Gets called continuosly to update input and output operations.
/// </summary>
void loop()
{
    Led.update();
    Inputs.run();
    Telnet.loop();
    Actuator.run();
    HttpServer.handleClient();
}

#pragma endregion