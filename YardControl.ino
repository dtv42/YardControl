// --------------------------------------------------------------------------------------------------------------------
// <copyright file="YardControl.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:44 PM</created>
// <modified>21-5-2023 9:05 AM</modified>
// <author>Peter Trimmel</author>
// <summary>
//   This is the main sketch for the fiddle yard controller.
//   A minimal Arduino C/C++ program consists of only two functions:
//
//     setup() : This function is called once when a sketch starts after power - up or reset.
//               It is used to initialize variables, input and output pin modes, and other libraries 
//               needed in the sketch. It is analogous to the function main().
// 
//     loop()  : After setup() function exits (ends), the loop() function is executed repeatedly in 
//               the main program. It controls the board until the board is powered off or is reset.
//               It is analogous to a function while(1).
//    
//   This implementation is using a Raspberry Pi Pico W board using a RP2040, a 32-bit dual ARM Cortex-M0+ 
//   microcontroller integrated circuit. The Arduino core available at https://github.com/earlephilhower/arduino-pico
//   provides several features some of them are used here:
//   
//      - Generic Arduino USB Serial
//      - SerialBT
//      - WiFi (Pico W)
//      - HTTP client and server (WebServer)
//      - Filesystems (LittleFS)
//      - digitalWrite/Read, temperature
//      - Wire (I2C)
//      - printf (i.e. debug) output over USB serial
// 
//   The following additional libraries are used:
// 
//      - ArduinoJson       https://github.com/bblanchon/ArduinoJson
//      - Blinkenlight      https://github.com/tfeldmann/Arduino-Blinkenlight
//      - InputDebounce     https://github.com/Mokolea/InputDebounce
//      - TimerInterrupt    https://github.com/khoih-prog/TimerInterrupt_Generic
//   
//   The Telnet implementation from https://github.com/LennartHennigs/ESPTelnet is used (ported and modified).
//   
//   Note that the LittleFS filesystem uses a part of the available flash memory.
//   
//      Flash Size: 2MB  (Sketch: 1536KB, FS: 512KB)
// 
//   To download the data files (all files and directories in the data directory) a little utility PicoLittleFS
//   (available at https://github.com/earlephilhower/arduino-pico-littlefs-plugin/releases) is used. This plugin
//   for the Arduino IDE only works with the legacy IDE (1.8.19).
// 
// </summary>
// --------------------------------------------------------------------------------------------------------------------
#if !(defined(ARDUINO_RASPBERRY_PI_PICO_W))
#error For RASPBERRY_PI_PICO_W only
#endif

#pragma region Includes

#include <SerialBT.h>
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
#include "src/UserInterface.h"

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

// Create the (global) user IO instance.
UserInterface UserIO;

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
    Serial.println(String("Telnet client connected: ") + ip);

    Telnet.println(HEADER);
    Telnet.println(COPYRIGHT);
    Telnet.println();
    Telnet.println("Welcome " + Telnet.getIP() + "");
    Telnet.print(Settings.Server.Prompt);
}

/// <summary>
/// Telnet disconnect event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetDisconnect(String ip)
{
    Serial.println(String("Telnet client disconnected: ") + ip);
}

/// <summary>
/// Telnet reconnect event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetReconnect(String ip)
{
    Serial.println(String("Telnet client reconnected: ") + ip);
}

/// <summary>
/// Telnet connection attempt event.
/// </summary>
/// <param name="ip">The IP address of the telnet client.</param>
void onTelnetConnectionAttempt(String ip)
{
    Serial.println(String("Telnet client connection attempt: ") + ip);
}

/// <summary>
/// Telnet input event. Process the input string and print the prompt.
/// </summary>
/// <param name="str">The input string.</param>
void onTelnetInput(String str)
{
    Commands.parse(str);
    Telnet.print(Settings.Server.Prompt);
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

#pragma region Initialize SerialBT

    SerialBT.begin();

    if (SerialBT) {
        SerialBT.println(HEADER);
        SerialBT.println(COPYRIGHT);
        SerialBT.print(">");
    }

#pragma endregion

#pragma region Initialize Settings

    // Initialize the file system and the application settings.
    LittleFS.begin();
    Settings.load();

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
        // Show WiFi info.
        WiFiInfo wifiInfo;
        Serial.print(wifiInfo.toString());

        // Set the clock using NTP.
        wireless.setClock();
        Serial.println(String("Current UTC time: ") + wireless.getTime());
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

    HttpServer.begin(Settings.Server.Http);

#pragma endregion

#pragma region Initialize Telnet

    // Passing on functions for various Telnet events.
    Telnet.onConnect(onTelnetConnect);
    Telnet.onConnectionAttempt(onTelnetConnectionAttempt);
    Telnet.onReconnect(onTelnetReconnect);
    Telnet.onDisconnect(onTelnetDisconnect);
    Telnet.onInputReceived(onTelnetInput);

    if (!Telnet.begin(Settings.Server.Telnet))
    {
        Serial.println("Setup failed!");

        // Don't continue, just indicate the error.
        Led.pattern(3, 3);

        while (true)
            Led.update();
    }

    // Show TCP server info.
    ServerInfo serverInfo;
    Serial.print(serverInfo.toString());

#pragma endregion

#pragma region Initialize Timer

    // Initialize timer with maximum frequency (100 kHz).
    if (Timer.setFrequency(Actuator.FREQUENCY, TimerHandler))
    {
        Serial.println("Starting Stepper Timer OK");
    }
    else
    {
        Serial.println("Can't set Stepper Timer.");

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
    UserIO.run();
    Telnet.loop();
    HttpServer.handleClient();
}

#pragma endregion












