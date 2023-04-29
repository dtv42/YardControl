// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>21-4-2023 12:56 PM</created>
// <modified>25-4-2023 4:28 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#include "src/AppSettings.h"
#include "src/TelnetServer.h"
#include "src/Actuator.h"
#include "src/GpioInputs.h"

#include "src/ActuatorInfo.h"
#include "src/ServerInfo.h"
#include "src/SystemInfo.h"
#include "src/WiFiInfo.h"
#include "src/PicoPins.h"

#include "src/Commands.h"

#pragma region Standard Callbacks

/// <summary>
/// Toggle the json flag (output format).
/// </summary>
void json()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("json()");
    Commands.JsonOutput = !Commands.JsonOutput;
}

/// <summary>
/// The quit command waits for a response
/// </summary>
void quit()
{
    TRACE();

    if (Commands.VerboseOutput) Telnet.println("quit()");

    if (Commands.WaitForResponse)
    {
        Telnet.println("Bye...");
        Telnet.disconnectClient();
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
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("help()");
    Telnet.println(Commands.getHelp());
}

/// <summary>
/// Toggle the verbose flag (output).
/// </summary>
void verbose()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("verbose()");
    Commands.VerboseOutput = !Commands.VerboseOutput;
}

/// <summary>
/// Prints an error message. This is one of the required global functions.
/// </summary>
/// <param name="message">The error message to be displayed.</param>
void error(String message)
{
    TRACE();
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
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("status()");
    ActuatorInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the current position.
/// </summary>
void position()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("position()");
    Telnet.println(String("position: ") + Actuator.getPosition());
}

/// <summary>
/// Move a single step forward (relative).
/// </summary>
void plus()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("plus()");
    Actuator.moveRelative(1);
}

/// <summary>
/// Move a single step backward (relative).
/// </summary>
void minus()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("minus()");
    Actuator.moveRelative(-1);
}

/// <summary>
/// Move a small distance [mm] forward (relative).
/// </summary>
void forward()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("forward()");
    Actuator.moveRelativeDistance(0.1);
}

/// <summary>
/// Move a small distance [mm] backward (relative).
/// </summary>
void backward()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("backward()");
    Actuator.moveRelativeDistance(-0.1);
}

/// <summary>
/// Run the calibration routine.
/// </summary>
void calibrate()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("calibrate()");
    Actuator.calibrate();
}

/// <summary>
/// Enable the stepper motor outputs.
/// </summary>
void enable()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("enable()");
    Actuator.enable();
}

/// <summary>
/// Disable the stepper motor outputs.
/// </summary>
void disable()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("disable()");
    Actuator.disable();
}

/// <summary>
/// Stop the stepper motor (immediately).
/// </summary>
void stop()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("stop()");
    Actuator.stop();
}

/// <summary>
/// Release the stepper motor from stop.
/// </summary>
void release()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("release()");
    Actuator.release();
}

/// <summary>
/// Reset the stepper motor position.
/// </summary>
void reset()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("reset()");
    Actuator.reset();
}

/// <summary>
/// Save the current application settings updating max. speed and acceleration.
/// </summary>
void save()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("save()");
    Settings.Stepper.MaxSpeed = Actuator.getMaxSpeed();
    Settings.Stepper.Acceleration = Actuator.getAcceleration();
    Settings.save();
}

/// <summary>
/// Move to home (zero).
/// </summary>
void home()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("home()");
    Actuator.home();
}

/// <summary>
/// Print the GPIO pin states.
/// </summary>
void gpio()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("gpio()");
    Commands.JsonOutput ? Telnet.print(Pins.toJsonString()) : Telnet.print(Pins.toString());
}

/// <summary>
/// Print the current speed [steps per second].
/// </summary>
void speed()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("speed()");
    Telnet.println(String(Actuator.getSpeed()));
}

/// <summary>
/// Print the maximum speed [steps per second].
/// </summary>
void maxspeed()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("maxspeed()");
    Telnet.println(String(Actuator.getMaxSpeed()));
}

/// <summary>
/// Print the acceleration [steps per second per second].
/// </summary>
void acceleration()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("acceleration()");
    Telnet.println(String(Actuator.getAcceleration()));
}

/// <summary>
/// Prints the minimum pulsewidth [microseconds].
/// </summary>
void pulsewidth()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("pulsewidth()");
    Telnet.println(String(Actuator.getPulseWidth()));
}

/// <summary>
/// Prints the microsteps.
/// </summary>
void microsteps()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("microsteps()");
    Telnet.println(String(Actuator.getMicroSteps()));
}

// Number command functions (callbacks).

/// <summary>
/// Retracts a short distance.
/// </summary>
/// <param name="value">The direction (+1 or -1).</param>
void retract(long value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("retract(") + value + ")");
    Actuator.retract(value);
}

/// <summary>
/// Move to absolute distance [mm].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsoluteDistance(float value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("moveAbsoluteDistance(") + value + ")");
    Actuator.moveAbsoluteDistance(value);
}

/// <summary>
/// Move a relative distance [mm].
/// </summary>
/// <param name="value">The distance to be moved.</param>
void moveRelativeDistance(float value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("moveRelativeDistance(") + value + ")");
    Actuator.moveRelativeDistance(value);
}

/// <summary>
/// Move to absolute distance [steps].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsolute(long value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("moveAbsolute(") + value + ")");
    Actuator.moveAbsolute(value);
}

/// <summary>
/// Move a relative distance [steps].
/// </summary>
/// <param name="value">The steps to be moved.</param>
void moveRelative(long value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("moveRelative(") + value + ")");
    Actuator.moveRelative(value);
}

/// <summary>
/// Move to specified track.
/// </summary>
/// <param name="value">The track number.</param>
void moveToTrack(long value)
{
    TRACE(); DUMP(value);
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
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("speed(") + value + ")");
    Actuator.setSpeed(value);
}

/// <summary>
/// Set the maximum speed [steps per second].
/// </summary>
/// <param name="value">The new maximum speed value.</param>
void maxspeed(float value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("maxspeed(") + value + ")");
    Actuator.setMaxSpeed(value);
}

/// <summary>
/// Set the acceleration [steps per second per second].
/// </summary>
/// <param name="value">The new acceleration value.</param>
void acceleration(float value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("acceleration(") + value + ")");
    Actuator.setAcceleration(value);
}

/// <summary>
/// Sets the minimum pulsewidth [microseconds].
/// </summary>
/// <param name="value">The new pulse width.</param>
void pulsewidth(long value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("pulsewidth(") + value + ")");
    Settings.Stepper.MinPulseWidth = value;
    Actuator.setPulseWidth(value);
}

/// <summary>
/// Sets the microsteps.
/// </summary>
/// <param name="value">The new microsteps.</param>
void microsteps(long value)
{
    TRACE(); DUMP(value);
    if (Commands.VerboseOutput) Telnet.println(String("microsteps(") + value + ")");
    Settings.Stepper.MicroSteps = value;
    Actuator.setMicroSteps(value);
}

// Basic command functions (callbacks).

/// <summary>
/// Print the track settings.
/// </summary>
void yard()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("yard()");
    Commands.JsonOutput ? Telnet.print(Settings.Yard.toJsonString()) : Telnet.print(Settings.Yard.toString());
}

/// <summary>
/// Print the Pico W pin layout.
/// </summary>
void pico()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("pico()");
    Telnet.print(PICO_W_GPIO);
}

/// <summary>
/// Print the WiFi status information.
/// </summary>
void wifi()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("wifi()");
    WiFiInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the system information.
/// </summary>
void system()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("system()");
    SystemInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the server (Http, Telnet) info.
/// </summary>
void server()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("server()");
    ServerInfo info;
    Commands.JsonOutput ? Telnet.print(info.toJsonString()) : Telnet.print(info.toString());
}

/// <summary>
/// Print the application settings.
/// </summary>
void settings()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("settings()");
    Commands.JsonOutput ? Telnet.print(Settings.toJsonString()) : Telnet.print(Settings.toString());
}

/// <summary>
/// Print the application settings file.
/// </summary>
void appsettings()
{
    TRACE();
    if (Commands.VerboseOutput) Telnet.println("appsettings()");
    File file = LittleFS.open(SETTINGS_FILE, "r");
    Telnet.print(file.readString());
    file.close();
}

/// <summary>
/// Reboot the system. This waits for a response (Telnet) for confirmation.
/// </summary>
void reboot()
{
    TRACE();
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

