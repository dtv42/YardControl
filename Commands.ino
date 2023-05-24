// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>21-4-2023 12:56 PM</created>
// <modified>21-5-2023 9:05 AM</modified>
// <author>Peter Trimmel</author>
// <summary>
//   All callbacks for the command processor can be found here. This file is merged with the other .ino files.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

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
    Commands.JsonOutput = !Commands.JsonOutput;
}

/// <summary>
/// The quit command waits for a response
/// </summary>
void quit()
{
    if (Commands.WaitForResponse)
    {
        if (Telnet.isConnected())
        {
            UserIO.show("Bye...");
            Telnet.disconnectClient();
        }
    }
    else
    {
        UserIO.show("Do You really want to quit (Y/N)? ");
        Commands.WaitForResponse = true;
    }
}

/// <summary>
/// Prints the help message.
/// </summary>
void help()
{
    UserIO.show(Commands.getHelp());
}

/// <summary>
/// Toggle the verbose flag.
/// </summary>
void verbose()
{
    UserIO.toggleVerbose();
}

/// <summary>
/// Prints an error message. This is one of the required global functions.
/// </summary>
/// <param name="message">The error message to be displayed.</param>
void error(String message)
{
    UserIO.println(message);
}

/// <summary>
/// Does nothing. This is one of the required global functions.
/// </summary>
void nop() {}

#pragma endregion

#pragma region BaseCommand Callbacks

/// <summary>
/// Print the actuator info.
/// </summary>
void status()
{
    Commands.JsonOutput ? UserIO.show(Actuator.toJsonString()) : UserIO.show(Actuator.toString());
}

/// <summary>
/// Print the current position.
/// </summary>
void position()
{
    UserIO.show(String("position: ") + Actuator.getPosition());
}

/// <summary>
/// Move a single step forward (relative).
/// </summary>
void plus()
{
    UserIO.println(Actuator.moveRelativeDistance(Settings.Actuator.MinStep));
}

/// <summary>
/// Move a single step backward (relative).
/// </summary>
void minus()
{
    UserIO.println(Actuator.moveRelativeDistance(-Settings.Actuator.MinStep));
}

/// <summary>
/// Move a small distance [mm] forward (relative).
/// </summary>
void forward()
{
    UserIO.println(Actuator.moveRelativeDistance(Settings.Actuator.SmallStep));
}

/// <summary>
/// Move a small distance [mm] backward (relative).
/// </summary>
void backward()
{
    UserIO.println(Actuator.moveRelativeDistance(-Settings.Actuator.SmallStep));
}

/// <summary>
/// Run the calibration routine.
/// </summary>
void calibrate()
{
    UserIO.println(Actuator.calibrate());
}

/// <summary>
/// Enable the stepper motor outputs.
/// </summary>
void enable()
{
    Actuator.enable();
}

/// <summary>
/// Disable the stepper motor outputs.
/// </summary>
void disable()
{
    Actuator.disable();
}

/// <summary>
/// Stop the stepper motor (immediately).
/// </summary>
void stop()
{
    Actuator.stop();
}

/// <summary>
/// Reset the stepper motor position.
/// </summary>
void reset()
{
    UserIO.println(Actuator.reset());
}

/// <summary>
/// Saves the current application settings.
/// The actuator specific settings are updated before saving.
/// </summary>
void save()
{
    Actuator.update();

    bool ok = Settings.save();

    if (ok) 
    {
        UserIO.println("Current application settings saved.");
    }
    else
    {
        UserIO.println("Unable to save application settings.");
    }
}

/// <summary>
/// (Re)loads the application settings.
/// The actuator is initialized with the actuator specific settings.
/// </summary>
void load()
{
    bool ok = Settings.load();

    if (ok)
    {
        Actuator.init();

        UserIO.println("Current application settings loaded.");
    }
    else
    {
        UserIO.println("Unable to load application settings.");
    }
}

/// <summary>
/// Move to home (zero).
/// </summary>
void home()
{
        UserIO.println(Actuator.home());
}

/// <summary>
/// Print the GPIO pin states.
/// </summary>
void gpio()
{
    Commands.JsonOutput ? UserIO.show(Pins.toJsonString()) : UserIO.show(Pins.toString());
}

/// <summary>
/// Print the small step distance [mm].
/// </summary>
void smallstep()
{
    UserIO.show(String(Actuator.getSmallStep()));
}

/// <summary>
/// Print the min step distance [mm].
/// </summary>
void minstep()
{
    UserIO.show(String(Actuator.getMinStep()));
}

/// <summary>
/// Print the retract distance [mm].
/// </summary>
void retract()
{
    UserIO.show(String(Actuator.getRetract()));
}

/// <summary>
/// Print the current speed [RPM].
/// </summary>
void rpm()
{
    UserIO.show(String(Actuator.getRPM()));
}

/// <summary>
/// Print the current speed [steps per second].
/// </summary>
void speed()
{
    UserIO.show(String(Actuator.getSpeed()));
}

/// <summary>
/// Print the minimum speed [steps per second].
/// </summary>
void minspeed()
{
    UserIO.show(String(Actuator.getMinSpeed()));
}

/// <summary>
/// Print the maximum speed [steps per second].
/// </summary>
void maxspeed()
{
    UserIO.show(String(Actuator.getMaxSpeed()));
}

/// <summary>
/// Print the maximum ramp steps.
/// </summary>
void maxsteps()
{
    UserIO.show(String(Actuator.getMaxSteps()));
}

/// <summary>
/// Prints the microsteps.
/// </summary>
void microsteps()
{
    UserIO.show(String(Actuator.getMicrosteps()));
}

// Number command functions (callbacks).

/// <summary>
/// Retracts a short distance in the opposite direction (used to move away from stop).
/// </summary>
void moveAway()
{
    UserIO.println(Actuator.moveAway());
}

/// <summary>
/// Move to absolute distance [mm].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsoluteDistance(float value)
{
    UserIO.println(Actuator.moveAbsoluteDistance(value));
}

/// <summary>
/// Move a relative distance [mm].
/// </summary>
/// <param name="value">The distance to be moved.</param>
void moveRelativeDistance(float value)
{
    UserIO.println(Actuator.moveRelativeDistance(value));
}

/// <summary>
/// Move to absolute distance [steps].
/// </summary>
/// <param name="value">The position to be moved to.</param>
void moveAbsolute(long value)
{
    UserIO.println(Actuator.moveAbsolute(value));
}

/// <summary>
/// Move a relative distance [steps].
/// </summary>
/// <param name="value">The steps to be moved.</param>
void moveRelative(long value)
{
    UserIO.println(Actuator.moveRelative(value));
}

/// <summary>
/// Move to specified track.
/// </summary>
/// <param name="value">The track number.</param>
void moveToTrack(long value)
{
    UserIO.println(Actuator.moveTrack(value));
}

/// <summary>
/// Set the small step distance [mm].
/// </summary>
/// <param name="value">The new distance value.</param>
void smallstep(float value)
{
    UserIO.println(Actuator.setSmallStep(value));
}

/// <summary>
/// Set the min step distance [mm].
/// </summary>
/// <param name="value">The new distance value.</param>
void minstep(float value)
{
    UserIO.println(Actuator.setMinStep(value));
}

/// <summary>
/// Set the retract distance [mm].
/// </summary>
/// <param name="value">The new distance value.</param>
void retract(float value)
{
    UserIO.println(Actuator.setRetract(value));
}

/// <summary>
/// Set the minimum speed [steps per seconds].
/// </summary>
/// <param name="value">The new speed value.</param>
void minspeed(float value)
{
    UserIO.println(Actuator.setMinSpeed(value));
}

/// <summary>
/// Set the maximum speed [steps per seconds].
/// </summary>
/// <param name="value">The new speed value.</param>
void maxspeed(float value)
{
    UserIO.println(Actuator.setMaxSpeed(value));
}

/// <summary>
/// Set the maximum ramp steps.
/// </summary>
/// <param name="value">The new ramp steps value.</param>
void maxsteps(long value)
{
    UserIO.println(Actuator.setMaxSteps(value));
}

/// <summary>
/// Set the microsteps.
/// </summary>
/// <param name="value">The new microsteps value.</param>
void microsteps(long value)
{
    UserIO.println(Actuator.setMicrosteps(value));
}

// Basic command functions (callbacks).

/// <summary>
/// Print the track settings.
/// </summary>
void yard()
{
    Commands.JsonOutput ? UserIO.show(Settings.Yard.toJsonString()) : UserIO.show(Settings.Yard.toString());
}

/// <summary>
/// Print the Pico W pin layout.
/// </summary>
void pico()
{
    UserIO.show(PICO_W_GPIO);
}

/// <summary>
/// Print the WiFi status information.
/// </summary>
void wifi()
{
    WiFiInfo info;
    Commands.JsonOutput ? UserIO.show(info.toJsonString()) : UserIO.show(info.toString());
}

/// <summary>
/// Print the system information.
/// </summary>
void system()
{
    SystemInfo info;
    Commands.JsonOutput ? UserIO.show(info.toJsonString()) : UserIO.show(info.toString());
}

/// <summary>
/// Print the server (Http, Telnet) info.
/// </summary>
void server()
{
    ServerInfo info;
    Commands.JsonOutput ? UserIO.show(info.toJsonString()) : UserIO.show(info.toString());
}

/// <summary>
/// Print the stepper settings.
/// </summary>
void stepper()
{
    Commands.JsonOutput ? UserIO.show(Settings.Stepper.toJsonString()) : UserIO.show(Settings.Stepper.toString());
}

/// <summary>
/// Print the actuator settings.
/// </summary>
void actuator()
{
    Commands.JsonOutput ? UserIO.show(Settings.Actuator.toJsonString()) : UserIO.show(Settings.Actuator.toString());
}

/// <summary>
/// Print the all settings.
/// </summary>
void settings()
{
    Commands.JsonOutput ? UserIO.show(Settings.toJsonString()) : UserIO.show(Settings.toString());
}

/// <summary>
/// Print the application settings file.
/// </summary>
void appsettings()
{
    File file = LittleFS.open(SETTINGS_FILE, "r");
    UserIO.show(file.readString());
    file.close();
}

/// <summary>
/// Reboot the system. This waits for a response (Telnet) for confirmation.
/// </summary>
void reboot()
{
    if (Commands.WaitForResponse)
    {      
        if (Telnet.isConnected())
        {
            UserIO.show("Rebooting...");
            Telnet.disconnectClient();
        }

        rp2040.reboot();
    }
    else
    {
        UserIO.show("Do You really want to reboot (Y/N)? ");
        Commands.WaitForResponse = true;
    }
}

#pragma endregion





