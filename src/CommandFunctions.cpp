﻿// --------------------------------------------------------------------------------------------------------------------
// <copyright file="CommandFunctions.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>21-4-2023 12:56 PM</created>
// <modified>21-4-2023 2:06 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "Commands.h"
#include "AppSettings.h"
#include "TelnetServer.h"
#include "ActuatorInfo.h"
#include "ServerInfo.h"
#include "SystemInfo.h"
#include "WiFiInfo.h"
#include "Actuator.h"
#include "PicoPins.h"
#include "GpioInputs.h"

extern AppSettings Settings;
extern TelnetServer Telnet;
extern CommandsClass Commands;
extern LinearActuator Actuator;
extern GpioPins Pins;

#pragma region Standard Callbacks

/// <summary>
/// Toggle the json flag (output format).
/// </summary>
void json()
{
    if (Commands.VerboseOutput) Telnet.println("json()");
    Commands.JsonOutput = !Commands.JsonOutput;
}

void quit()
{
    if (Commands.VerboseOutput) Telnet.println("quit()");

    if (Commands.WaitForResponse)
    {
        Telnet.println("Bye...");
    }
    else
    {
        Serial.print("Do You really want to quit (Y/N)? ");
        Commands.WaitForResponse = true;
    }
}

/// <summary>
/// Prints the help message.
/// </summary>
void help()
{
    if (Commands.VerboseOutput) Telnet.println("help()");
    Telnet.println(Commands.getHelp());
}

/// <summary>
/// Toggle the verbose flag (output).
/// </summary>
void verbose()
{
    if (Commands.VerboseOutput) Telnet.println("verbose()");
    Commands.VerboseOutput = !Commands.VerboseOutput;
}

/// <summary>
/// Prints an error message. This is one of the required global functions.
/// </summary>
/// <param name="message">The error message to be displayed.</param>
void error(String message)
{
    if (Commands.VerboseOutput) Telnet.println(String("error(") + message + ")");
    Telnet.println(message);
}

/// <summary>
/// Does nothing. This is one of the required global functions.
/// </summary>
void nop()
{
    if (Commands.VerboseOutput) Telnet.println("nop()");
}

#pragma endregion

#pragma region BaseCommand Callbacks

/// <summary>
/// Print the actuator info.
/// </summary>
void status()
{
    if (Commands.VerboseOutput) Telnet.println("status()");
    ActuatorInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the current position.
/// </summary>
void position()
{
    if (Commands.VerboseOutput) Telnet.println("position()");
    Telnet.println(String("position: ") + Actuator.getPosition());
}

/// <summary>
/// Move a single step forward (relative).
/// </summary>
void plus()
{
    if (Commands.VerboseOutput) Telnet.println("plus()");
    Actuator.moveRelative(1);
}

/// <summary>
/// Move a single step backward (relative).
/// </summary>
void minus()
{
    if (Commands.VerboseOutput) Telnet.println("minus()");
    Actuator.moveRelative(-1);
}

/// <summary>
/// Move a small distance [mm] forward (relative).
/// </summary>
void forward()
{
    if (Commands.VerboseOutput) Telnet.println("forward()");
    Actuator.moveRelativeDistance(0.1);
}

/// <summary>
/// Move a small distance [mm] backward (relative).
/// </summary>
void backward()
{
    if (Commands.VerboseOutput) Telnet.println("backward()");
    Actuator.moveRelativeDistance(-0.1);
}

/// <summary>
/// Run the calibration routine.
/// </summary>
void calibrate()
{
    if (Commands.VerboseOutput) Telnet.println("calibrate()");
    Actuator.calibrate();
}

/// <summary>
/// Enable the stepper motor outputs.
/// </summary>
void enable()
{
    if (Commands.VerboseOutput) Telnet.println("enable()");
    Actuator.enable();
}

/// <summary>
/// Disable the stepper motor outputs.
/// </summary>
void disable()
{
    if (Commands.VerboseOutput) Telnet.println("disable()");
    Actuator.disable();
}

/// <summary>
/// Stop the stepper motor (immediately).
/// </summary>
void stop()
{
    if (Commands.VerboseOutput) Telnet.println("stop()");
    Actuator.stop();
}

/// <summary>
/// Release the stepper motor from stop.
/// </summary>
void release()
{
    if (Commands.VerboseOutput) Telnet.println("release()");
    Actuator.release();
}

/// <summary>
/// Reset the stepper motor position.
/// </summary>
void reset()
{
    if (Commands.VerboseOutput) Telnet.println("reset()");
    Actuator.reset();
}

/// <summary>
/// Move to home (zero).
/// </summary>
void home()
{
    if (Commands.VerboseOutput) Telnet.println("home()");
    Actuator.home();
}

/// <summary>
/// Print the GPIO pin states.
/// </summary>
void gpio()
{
    if (Commands.VerboseOutput) Telnet.println("gpio()");
    Commands.JsonOutput ? Telnet.print(Pins.toJsonString()) : Telnet.print(Pins.toString());
}

/// <summary>
/// Print the current speed [steps per second].
/// </summary>
void speed()
{
    if (Commands.VerboseOutput) Telnet.println("speed()");
    Telnet.print(Actuator.getSpeed());
}

/// <summary>
/// Print the maximum speed [steps per second].
/// </summary>
void maxspeed()
{
    if (Commands.VerboseOutput) Telnet.println("maxspeed()");
    Telnet.print(Actuator.getMaxSpeed());
}

/// <summary>
/// Print the acceleration [steps per second per second].
/// </summary>
void acceleration()
{
    if (Commands.VerboseOutput) Telnet.println("acceleration()");
    Telnet.print(Actuator.getAcceleration());
}

// Number command functions (callbacks).

/// <summary>
/// Move to absolute distance [mm].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsoluteDistance(float value)
{
    if (Commands.VerboseOutput) Telnet.println(String("moveAbsoluteDistance(") + value + ")");
    Actuator.moveAbsoluteDistance(value);
}

/// <summary>
/// Move a relative distance [mm].
/// </summary>
/// <param name="value">The distance to be moved.</param>
void moveRelativeDistance(float value)
{
    if (Commands.VerboseOutput) Telnet.println(String("moveRelativeDistance(") + value + ")");
    Actuator.moveRelativeDistance(value);
}

/// <summary>
/// Move to absolute distance [steps].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsolute(long value)
{
    if (Commands.VerboseOutput) Telnet.println(String("moveAbsolute(") + value + ")");
    Actuator.moveAbsolute(value);
}

/// <summary>
/// Move a relative distance [steps].
/// </summary>
/// <param name="value">The steps to be moved.</param>
void moveRelative(long value)
{
    if (Commands.VerboseOutput) Telnet.println(String("moveRelative(") + value + ")");
    Actuator.moveRelative(value);
}

/// <summary>
/// Move to specified track.
/// </summary>
/// <param name="value">The track number.</param>
void moveToTrack(long value)
{
    if (Commands.VerboseOutput) Telnet.println(String("moveToTrack(") + value + ")");
    if (value >= 0 && value < Settings.Yard.Tracks.size())
        Actuator.moveAbsolute(Settings.Yard.Tracks[value]);
}

/// <summary>
/// Set the current speed [steps per second].
/// </summary>
/// <param name="value">The new speed value.</param>
void speed(float value)
{
    if (Commands.VerboseOutput) Telnet.println(String("speed(") + value + ")");
    Actuator.setSpeed(value);
}

/// <summary>
/// Set the maximum speed [steps per second].
/// </summary>
/// <param name="value">The new maximum speed value.</param>
void maxspeed(float value)
{
    if (Commands.VerboseOutput) Telnet.println(String("maxspeed(") + value + ")");
    Actuator.setMaxSpeed(value);
}

/// <summary>
/// Set the acceleration [steps per second per second].
/// </summary>
/// <param name="value">The new acceleration value.</param>
void acceleration(float value)
{
    if (Commands.VerboseOutput) Telnet.println(String("acceleration(") + value + ")");
    Actuator.setAcceleration(value);
}

// Basic command functions (callbacks).

/// <summary>
/// Print the Pico W pin layout.
/// </summary>
void pico()
{
    if (Commands.VerboseOutput) Telnet.println("pico()");
    Telnet.print(PICO_W_GPIO);
}

/// <summary>
/// Print the WiFi status information.
/// </summary>
void wifi()
{
    if (Commands.VerboseOutput) Telnet.println("wifi()");
    WiFiInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the system information.
/// </summary>
void system()
{
    if (Commands.VerboseOutput) Telnet.println("system()");
    SystemInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the server (Http, Telnet) info.
/// </summary>
void server()
{
    if (Commands.VerboseOutput) Telnet.println("server()");
    ServerInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the application settings.
/// </summary>
void settings()
{
    if (Commands.VerboseOutput) Telnet.println("settings()");
    Commands.JsonOutput ? Telnet.print(Settings.toJsonString()) : Telnet.print(Settings.toString());
}

/// <summary>
/// Reboot the system. This waits for a response (Telnet) for confirmation.
/// </summary>
void reboot()
{
    if (Commands.VerboseOutput) Telnet.println("reboot()");

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

#pragma endregion

