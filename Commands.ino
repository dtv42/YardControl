// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>21-4-2023 12:56 PM</created>
// <modified>10-5-2023 9:41 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

// Disable all traces when set to 0.
#define ARDUINOTRACE_ENABLE 0

#include "src/AppSettings.h"
#include "src/TelnetServer.h"
#include "src/Actuator.h"
#include "src/GpioInputs.h"

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
    Commands.JsonOutput = !Commands.JsonOutput;
}

/// <summary>
/// The quit command waits for a response
/// </summary>
void quit()
{
    TRACE();

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
    Telnet.println(Commands.getHelp());
}

/// <summary>
/// Toggle the verbose flag.
/// </summary>
void verbose()
{
    TRACE();
    Actuator.setVerboseFlag(!Actuator.getVerboseFlag());
}

/// <summary>
/// Prints an error message. This is one of the required global functions.
/// </summary>
/// <param name="message">The error message to be displayed.</param>
void error(String message)
{
    TRACE();
    Telnet.println(message);
}

/// <summary>
/// Does nothing. This is one of the required global functions.
/// </summary>
void nop()
{
    TRACE();
}

#pragma endregion

#pragma region BaseCommand Callbacks

/// <summary>
/// Print the actuator info.
/// </summary>
void status()
{
    TRACE();
    Commands.JsonOutput ? Telnet.print(Actuator.toJsonString()) : Telnet.print(Actuator.toString());
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
    Actuator.moveRelativeDistance(Settings.Actuator.MinStep);
}

/// <summary>
/// Move a single step backward (relative).
/// </summary>
void minus()
{
    TRACE();
    Actuator.moveRelativeDistance(-Settings.Actuator.MinStep);
}

/// <summary>
/// Move a small distance [mm] forward (relative).
/// </summary>
void forward()
{
    TRACE();
    Actuator.moveRelativeDistance(Settings.Actuator.SmallStep);
}

/// <summary>
/// Move a small distance [mm] backward (relative).
/// </summary>
void backward()
{
    TRACE();
    Actuator.moveRelativeDistance(-Settings.Actuator.SmallStep);
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
/// Reset the stepper motor position.
/// </summary>
void reset()
{
    TRACE();
    Actuator.reset();
}

/// <summary>
/// Saves the current application settings.
/// </summary>
void save()
{
    TRACE();
    Settings.save();
}

/// <summary>
/// (Re)loads the application settings.
/// </summary>
void load()
{
    TRACE();
    Settings.load();
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
    Commands.JsonOutput ? Telnet.print(Pins.toJsonString()) : Telnet.print(Pins.toString());
}

/// <summary>
/// Print the small step distance [mm].
/// </summary>
void smallstep()
{
    TRACE();
    Telnet.println(String(Settings.Actuator.SmallStep));
}

/// <summary>
/// Print the min step distance [mm].
/// </summary>
void minstep()
{
    TRACE();
    Telnet.println(String(Settings.Actuator.MinStep));
}

/// <summary>
/// Print the retract distance [mm].
/// </summary>
void retract()
{
    TRACE();
    Telnet.println(String(Settings.Actuator.Retract));
}

/// <summary>
/// Print the current speed [RPM].
/// </summary>
void rpm()
{
    TRACE();
    Telnet.println(String(Actuator.getRPM()));
}

/// <summary>
/// Print the current speed [steps per second].
/// </summary>
void speed()
{
    TRACE();
    Telnet.println(String(Actuator.getSpeed()));
}

/// <summary>
/// Print the maximum speed [steps per second].
/// </summary>
void maxspeed()
{
    TRACE();
    Telnet.println(String(Actuator.getMaxSpeed()));
}

/// <summary>
/// Print the maximum ramp steps.
/// </summary>
void maxsteps()
{
    TRACE();
    Telnet.println(String(Actuator.getMaxSteps()));
}

/// <summary>
/// Prints the microsteps.
/// </summary>
void microsteps()
{
    TRACE();
    Telnet.println(String(Actuator.getMicrosteps()));
}

// Number command functions (callbacks).

/// <summary>
/// Retracts a short distance.
/// </summary>
/// <param name="value">The direction (+1 or -1).</param>
void retract(long value)
{
    TRACE(); DUMP(value);
    if ((value == 1) || (value == -1))
        Actuator.retract(static_cast<LinearActuator::Direction>(value));
}

/// <summary>
/// Move to absolute distance [mm].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsoluteDistance(float value)
{
    TRACE(); DUMP(value);
    Actuator.moveAbsoluteDistance(value);
}

/// <summary>
/// Move a relative distance [mm].
/// </summary>
/// <param name="value">The distance to be moved.</param>
void moveRelativeDistance(float value)
{
    TRACE(); DUMP(value);
    Actuator.moveRelativeDistance(value);
}

/// <summary>
/// Move to absolute distance [steps].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsolute(long value)
{
    TRACE(); DUMP(value);
    Actuator.moveAbsolute(value);
}

/// <summary>
/// Move a relative distance [steps].
/// </summary>
/// <param name="value">The steps to be moved.</param>
void moveRelative(long value)
{
    TRACE(); DUMP(value);
    Actuator.moveRelative(value);
}

/// <summary>
/// Move to specified track.
/// </summary>
/// <param name="value">The track number.</param>
void moveToTrack(long value)
{
    TRACE(); DUMP(value);
    if (value >= 0 && value < Settings.Yard.Tracks.size())
        Actuator.moveAbsolute(Settings.Yard.Tracks[value]);
}

/// <summary>
/// Set the small step distance [mm].
/// </summary>
/// <param name="value">The new distance value.</param>
void smallstep(float value)
{
    TRACE(); DUMP(value);
    Settings.Actuator.SmallStep = value;
}

/// <summary>
/// Set the min step distance [mm].
/// </summary>
/// <param name="value">The new distance value.</param>
void minstep(float value)
{
    TRACE(); DUMP(value);
    Settings.Actuator.MinStep = value;
}

/// <summary>
/// Set the retract distance [mm].
/// </summary>
/// <param name="value">The new distance value.</param>
void retract(float value)
{
    TRACE(); DUMP(value);
    Settings.Actuator.Retract = value;
}

/// <summary>
/// Set the maximum speed [steps per seconds].
/// </summary>
/// <param name="value">The new speed value.</param>
void maxspeed(float value)
{
    TRACE(); DUMP(value);
    Actuator.setMaxSpeed(value);
}

/// <summary>
/// Set the maximum ramp steps.
/// </summary>
/// <param name="value">The new ramp steps value.</param>
void maxsteps(long value)
{
    TRACE(); DUMP(value);
    Actuator.setMaxSteps(value);
}

/// <summary>
/// Set the microsteps.
/// </summary>
/// <param name="value">The new microsteps value.</param>
void microsteps(long value)
{
    TRACE(); DUMP(value);
    Actuator.setMicrosteps(value);
}

// Basic command functions (callbacks).

/// <summary>
/// Print the track settings.
/// </summary>
void yard()
{
    TRACE();
    Commands.JsonOutput ? Telnet.print(Settings.Yard.toJsonString()) : Telnet.print(Settings.Yard.toString());
}

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
/// Print the stepper settings.
/// </summary>
void stepper()
{
    TRACE();
    Commands.JsonOutput ? Telnet.print(Settings.Stepper.toJsonString()) : Telnet.print(Settings.Stepper.toString());
}

/// <summary>
/// Print the actuator settings.
/// </summary>
void actuator()
{
    TRACE();
    Commands.JsonOutput ? Telnet.print(Settings.Actuator.toJsonString()) : Telnet.print(Settings.Actuator.toString());
}

/// <summary>
/// Print the all settings.
/// </summary>
void settings()
{
    TRACE();
    Commands.JsonOutput ? Telnet.print(Settings.toJsonString()) : Telnet.print(Settings.toString());
}

/// <summary>
/// Print the application settings file.
/// </summary>
void appsettings()
{
    TRACE();
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

