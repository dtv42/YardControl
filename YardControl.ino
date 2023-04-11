// --------------------------------------------------------------------------------------------------------------------
// <copyright file="YardControl.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:44 PM</created>
// <modified>11-4-2023 9:45 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma region Includes

#include <ArduinoTrace.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <Blinkenlight.h>

#include <map>

#include "src/AppSettings.h"
#include "src/GpioPins.h"
#include "src/WiFiInfo.h"
#include "src/SystemInfo.h"
#include "src/ServerInfo.h"
#include "src/TelnetServer.h"
#include "src/ActuatorInfo.h"
#include "src/Wireless.h"
#include "src/Commands.h"
#include "src/Actuator.h"
#include "src/Defines.h"

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
    TRACE();

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
    TRACE();
    DUMP(HttpServer.uri());

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
    TRACE();
    sendFile(HttpServer.uri());
}

/// <summary>
/// Returns a JSON representation of various info items.
/// </summary>
void getInfo()
{
    TRACE();
    DUMP(HttpServer.uri());

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
            Pins.update();
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
    TRACE();
    DUMP(HttpServer.uri());

    if (HttpServer.method() != HTTP_POST)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        String command = HttpServer.uri().substring(1);

        if (Commands.isValidBaseCommand(command))
        {
            Commands.run(command);
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
    TRACE();
    DUMP(HttpServer.uri());

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
                    Commands.run(command);
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
    TRACE();
    DUMP(HttpServer.uri());

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
                    Commands.run(command);
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
    TRACE();

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
    TRACE();

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
    TRACE();

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
    TRACE();
    DUMP(ip);

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
    TRACE();
    DUMP(ip);
}

/// <summary>
/// Telnet reconnect event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetReconnect(String ip)
{
    TRACE();
    DUMP(ip);
}

/// <summary>
/// Telnet connection attempt event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetConnectionAttempt(String ip)
{
    TRACE();
    DUMP(ip);
}

/// <summary>
/// Telnet input event. Process the input string and print the prompt.
/// </summary>
/// <param name="str">The input string.</param>
void onTelnetInput(String str)
{
    TRACE();
    DUMP(str);
    Commands.run(str);
    Telnet.print(Settings.Telnet.Prompt);
}

#pragma endregion

#pragma region Command Callbacks

/// <summary>
/// Print the actuator info.
/// </summary>
void status()
{
    TRACE();
    ActuatorInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the current position.
/// </summary>
void position()
{
    TRACE();
    Telnet.println(String("position: ") + Actuator.getPosition());
}

/// <summary>
/// Move a single step forward (relative).
/// </summary>
void plus()
{
    TRACE();
    Actuator.moveRelative(1);
}

/// <summary>
/// Move a single step backward (relative).
/// </summary>
void minus()
{
    TRACE();
    Actuator.moveRelative(-1);
}

/// <summary>
/// Move a small distance [mm] forward (relative).
/// </summary>
void forward()
{
    TRACE();
    Actuator.moveRelativeDistance(0.1);
}

/// <summary>
/// Move a small distance [mm] backward (relative).
/// </summary>
void backward()
{
    TRACE();
    Actuator.moveRelativeDistance(-0.1);
}

/// <summary>
/// Run the calibration routine.
/// </summary>
void calibrate()
{
    TRACE();
    Actuator.calibrate();
}

/// <summary>
/// Enable the stepper motor outputs.
/// </summary>
void enable()
{
    TRACE();
    Actuator.enable();
}

/// <summary>
/// Disable the stepper motor outputs.
/// </summary>
void disable()
{
    TRACE();
    Actuator.disable();
}

/// <summary>
/// Stop the stepper motor (immediately).
/// </summary>
void stop()
{
    TRACE();
    Actuator.stop();
}

/// <summary>
/// Release the stepper motor from stop.
/// </summary>
void release()
{
    TRACE();
    Actuator.release();
}

/// <summary>
/// Reset the stepper motor position.
/// </summary>
void reset()
{
    TRACE();
    Actuator.reset();
}

/// <summary>
/// Move to home (zero).
/// </summary>
void home()
{
    TRACE();
    Actuator.home();
}

/// <summary>
/// Print the GPIO pin states.
/// </summary>
void gpio()
{
    TRACE();
    Pins.update();
    Commands.JsonOutput ? Telnet.print(Pins.toJsonString()) : Telnet.print(Pins.toString());
}

/// <summary>
/// Print the current speed [steps per second].
/// </summary>
void speed()
{
    TRACE();
    Telnet.print(Actuator.getSpeed());
}

/// <summary>
/// Print the maximum speed [steps per second].
/// </summary>
void maxspeed()
{
    TRACE();
    Telnet.print(Actuator.getMaxSpeed());
}

/// <summary>
/// Print the acceleration [steps per second per second].
/// </summary>
void acceleration()
{
    TRACE();
    Telnet.print(Actuator.getAcceleration());
}

/// <summary>
/// Toggle the json flag (output format).
/// </summary>
void json()
{
    TRACE();
    Commands.JsonOutput = !Commands.JsonOutput;
}

// Number command functions (callbacks).

/// <summary>
/// Move to absolute distance [mm].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsoluteDistance(float value)
{
    TRACE();
    DUMP(value);
    Actuator.moveAbsoluteDistance(value);
}

/// <summary>
/// Move a relative distance [mm].
/// </summary>
/// <param name="value">The distance to be moved.</param>
void moveRelativeDistance(float value)
{
    TRACE();
    DUMP(value);
    Actuator.moveRelativeDistance(value);
}

/// <summary>
/// Move to absolute distance [steps].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsolute(long value)
{
    TRACE();
    DUMP(value);
    Actuator.moveAbsolute(value);
}

/// <summary>
/// Move a relative distance [steps].
/// </summary>
/// <param name="value">The steps to be moved.</param>
void moveRelative(long value)
{
    TRACE();
    DUMP(value);
    Actuator.moveRelative(value);
}

/// <summary>
/// Move to specified track.
/// </summary>
/// <param name="value">The track number.</param>
void moveToTrack(long value)
{
    TRACE();
    DUMP(value);

    if (value >= 0 && value < Settings.Yard.Tracks.size())
        Actuator.moveAbsolute(Settings.Yard.Tracks[value]);
}

/// <summary>
/// Set the current speed [steps per second].
/// </summary>
/// <param name="value">The new speed value.</param>
void speed(float value)
{
    TRACE();
    DUMP(value);
    Actuator.setSpeed(value);
}

/// <summary>
/// Set the maximum speed [steps per second].
/// </summary>
/// <param name="value">The new maximum speed value.</param>
void maxspeed(float value)
{
    TRACE();
    DUMP(value);
    Actuator.setMaxSpeed(value);
}

/// <summary>
/// Set the acceleration [steps per second per second].
/// </summary>
/// <param name="value">The new acceleration value.</param>
void acceleration(float value)
{
    TRACE();
    DUMP(value);
    Actuator.setAcceleration(value);
}

// Basic command functions (callbacks).

/// <summary>
/// Print the Pico W pin layout.
/// </summary>
void pico()
{
    TRACE();
    Telnet.print(PICO_W_GPIO);
}

/// <summary>
/// Print the WiFi status information.
/// </summary>
void wifi()
{
    TRACE();
    WiFiInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the system information.
/// </summary>
void system()
{
    TRACE();
    SystemInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the server (Http, Telnet) info.
/// </summary>
void server()
{
    TRACE();
    ServerInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the application settings.
/// </summary>
void settings()
{
    TRACE();
    Commands.JsonOutput ? Telnet.print(Settings.toJsonString()) : Telnet.print(Settings.toString());
}

/// <summary>
/// Reboot the system. This waits for a response (Telnet) for confirmation.
/// </summary>
void reboot()
{
    TRACE();

    if (Commands.WaitForResponse)
    {
        Telnet.print("Rebooting...");
        Telnet.disconnectClient();
        rp2040.reboot();
    }
    else
    {
        Telnet.print("Do You really want to reboot (Y/N)? ");
        Commands.WaitForResponse = true;
    }
}

/// <summary>
/// Prints the command help.
/// </summary>
void help()
{
    TRACE();
    Telnet.print(Commands.help());
}

/// <summary>
/// Quits the Telnet session. This waits for a response (Telnet) for confirmation.
/// </summary>
void quit()
{
    TRACE();

    if (Commands.WaitForResponse)
    {
        Telnet.print("Bye...");
        Telnet.disconnectClient();
    }
    else
    {
        Telnet.print("Do You really want to quit (Y/N)? ");
        Commands.WaitForResponse = true;
    }
}

/// <summary>
/// Prints an error message.
/// </summary>
/// <param name="description">The error message to be displayed.</param>
void error(String description)
{
    TRACE();
    Telnet.println(description);
}

/// <summary>
/// Does nothing.
/// </summary>
void nop()
{
    TRACE();
}

#pragma endregion

#pragma region Switch Callbacks

/// <summary>
/// Callback triggered when the (debounced) switch is closed.
/// </summary>
/// <param name="pin">The input pin number.</param>
void pressedCallback(uint8_t pin)
{
    TRACE();
    DUMP(pin);
    Actuator.switchOn(pin);
}

/// <summary>
/// Callback triggered when the (debounced) switch is opened.
/// </summary>
/// <param name="pin">The input pin number.</param>
void releasedCallback(uint8_t pin)
{
    TRACE();
    DUMP(pin);
    Actuator.switchOff(pin);
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

#pragma region Initialize Actuator

    Pins.init();
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

#pragma region Initialize Commands

    Serial.println("Initializing commands");
    Commands.init(error, nop);

    // Base Commands (no shortcut).
    Commands.add(BaseCommand("pico",          "",  "Show Pico W pin layout.",                      pico));
    Commands.add(BaseCommand("wifi",          "",  "Shows the WiFi information.",                  wifi));
    Commands.add(BaseCommand("server",        "",  "Shows the server information.",                server));
    Commands.add(BaseCommand("system",        "",  "Shows the system information.",                system));
    Commands.add(BaseCommand("settings",      "",  "Shows the settings information.",              settings));
    Commands.add(BaseCommand("reboot",        "",  "Reboots the RP2040.",                          reboot));
    Commands.add(BaseCommand("reset",         "",  "Resets the current position to zero.",         reset));

    Commands.add(BaseCommand("speed",         "",  "Gets the current speed (steps/(sec*sec)).",    speed));
    Commands.add(BaseCommand("maxspeed",      "",  "Gets the maximum speed (steps/sec).",          maxspeed));
    Commands.add(BaseCommand("acceleration",  "",  "Gets the acceleration (steps/(sec*sec)).",     acceleration));

    // Base Commands (with shortcut).
    Commands.add(BaseCommand("status",        "s", "Shows the current state of the motor driver.", status));
    Commands.add(BaseCommand("position",      "p", "Shows the current position.",                  position));
    Commands.add(BaseCommand("plus",          "+", "Moves a step forward.",                        plus));
    Commands.add(BaseCommand("minus",         "-", "Moves a step backward.",                       minus));
    Commands.add(BaseCommand("forward",       "f", "Moves a 0.1 mm distance forward.",             forward));
    Commands.add(BaseCommand("backward",      "b", "Moves a 0.1 mm distance backward.",            backward));
    Commands.add(BaseCommand("calibrate",     "c", "Run a calibration sequence.",                  calibrate));
    Commands.add(BaseCommand("enable",        "e", "Enabling the output (after disable).",         enable));
    Commands.add(BaseCommand("disable",       "d", "Stops the motor by disabling the output.",     disable));
    Commands.add(BaseCommand("release",       "r", "Release the stopped motor.",                   release));
    Commands.add(BaseCommand("stop",          "x", "Stops the running motor (decelerating).",      stop));
    Commands.add(BaseCommand("home",          "h", "Moves to home position (position = 0).",       home));
    Commands.add(BaseCommand("gpio",          "g", "Shows the GPIO input and output pin values.",  gpio));
    Commands.add(BaseCommand("help",          "?", "Shows this help information.",                 help));
    Commands.add(BaseCommand("quit",          "q", "Terminates the program.",                      quit));
    Commands.add(BaseCommand("json",          "j", "Toggle JSON output.",                          json));

    // Argument Commands (no shortcut).
    Commands.add(FloatCommand("speed",        "",  "Sets the current speed. (steps/sec).",         speed));
    Commands.add(FloatCommand("maxspeed",     "",  "Sets the maximum speed. (steps/sec).",         maxspeed));
    Commands.add(FloatCommand("acceleration", "",  "Sets the acceleration (steps/(sec*sec)).",     acceleration));

    // Argument Commands (with shortcut).
    Commands.add(FloatCommand("moveto",       "a", "Moves to absolute position (mm).",             moveAbsoluteDistance));
    Commands.add(FloatCommand("move",         "r", "Moves the number of mm (relative).",           moveRelativeDistance));    
    Commands.add(LongCommand("stepto",        "m", "Moves to absolute position (steps).",          moveAbsolute));
    Commands.add(LongCommand("step",          "s", "Moves the number of steps (relative).",        moveRelative));
    Commands.add(LongCommand("track",         "t", "Moves to track number.",                       moveToTrack));

#pragma endregion

#pragma region Initialize Http

    // Web server setup - GET file
    HttpServer.on("/", getRoot);
    HttpServer.on("/about.html",                 getFile);
    HttpServer.on("/index.html",                 getFile);
    HttpServer.on("/info.html",                  getFile);
    HttpServer.on("/settings.html",              getFile);
    HttpServer.on("/favicon.ico",                getFile);
    HttpServer.on("/css/dark-mode-switch.css",   getFile);
    HttpServer.on("/js/dark-mode-switch.min.js", getFile);

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
    Telnet.loop();
    Actuator.run();
    HttpServer.handleClient();
}

#pragma endregion