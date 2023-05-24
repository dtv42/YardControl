// --------------------------------------------------------------------------------------------------------------------
// <copyright file="UserInterface.cpp" company="DTV-Online">                                                                        
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.                                                        
// </copyright>                                                                                                        
// <license>                                                                                                           
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.                    
// </license>                                                                                                          
// <created>05/23/2023 10:54:21</created>                                                                                                 
// <modified>05/24/2023 09:05:36</modified>                                                                                               
// <author>Peter Trimmel</author>                                                                                      
// <summary>                                                                                                           
//                                                                                                                     
// </summary>                                                                                                          
// --------------------------------------------------------------------------------------------------------------------

#include <SerialBT.h>

#include "UserInterface.h"
#include "TelnetServer.h"
#include "AppSettings.h"
#include "Actuator.h"
#include "Commands.h"

// Externals (globals).
extern AppSettings Settings;
extern TelnetServer Telnet;
extern CommandsClass Commands;
extern LinearActuator Actuator;

/// <summary>
/// Returns the verbose flag.
/// </summary>
/// <returns>The flag value.</returns>
bool UserInterface::getVerbose()
{
    return _verbose;
}

/// <summary>
/// Toggles the verbose flag.
/// </summary>
void UserInterface::toggleVerbose()
{
    _verbose = !_verbose;

    SerialBT.println(_verbose ? "Verbose on" : "Verbose off");
    Telnet.println(_verbose ? "Verbose on" : "Verbose off");
}

/// <summary>
/// Prints the string at telnet and SerialBT. Longer strings are splitted into smaller chunks for SerialBT output.
/// </summary>
/// <param name="str">The string to be printed.</param>
void UserInterface::show(const String& str)
{
    auto length = str.length();

    if (length > BTSTRING)
    {
        auto count = length / BTSTRING;
        auto final = length % BTSTRING;

        for (int i = 0; i < count; i++)
        {
            auto part = str.substring(i * BTSTRING, i * BTSTRING + BTSTRING);
            SerialBT.print(part);
            Telnet.print(part);
        }

        if (final > 0)
        {
            auto part = str.substring(count * BTSTRING);
            SerialBT.print(part);
        }
    }
    else
    {
        SerialBT.print(str);
    }

    Telnet.print(str);
}

/// <summary>
/// Prints just an empty line if verbose is enabled.
/// </summary>
void UserInterface::println()
{
    if (_verbose)
    {
        SerialBT.println();
        Telnet.println();
    }
}

/// <summary>
/// Prints the string if verbose is enabled (no linefeed added).
/// </summary>
/// <param name="str">The string to be printed.</param>
void UserInterface::print(const String& str)
{
    if (_verbose)
    {
        SerialBT.print(str);
        Telnet.print(str);
    }
}

/// <summary>
/// Prints the string if verbose is enabled (a linefeed is added).
/// Note that the string length seems to be limited (ca. 512) for SerialBT.
/// </summary>
/// <param name="str">The string to be printed.</param>
void UserInterface::println(const String& str)
{
    if (_verbose)
    {
        SerialBT.println(str);
        Telnet.println(str);
    }
}

/// <summary>
/// Update loop for printing move info, and getting SerialBT inputs.
/// </summary>
void UserInterface::run()
{
    String info = Actuator.getMoveInfo();

    if (_verbose)
    {
        if (info.length() > 0)
        {
            Telnet.println(info);
            Telnet.print(Settings.Server.Prompt);
        }
    }

    if (SerialBT)
    {
        if (_verbose)
        {
            if (info.length() > 0)
            {
                SerialBT.println(info);
                SerialBT.print(Settings.Server.Prompt);
            }
        }

        if (SerialBT.available()) {
            String line = SerialBT.readStringUntil('\n');
            if (line != "") {
                line.replace('\r', ' ');
                line.trim();
                Commands.parse(line);
                SerialBT.print(Settings.Server.Prompt);
            }
        }
    }
}























