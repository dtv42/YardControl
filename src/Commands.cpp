// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>11-4-2023 9:03 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <StringSplitter.h>

#include "TelnetServer.h"
#include "Commands.h"
#include "Defines.h"

/// <summary>
/// Telnet server to print the command results.
/// </summary>
extern TelnetServer Telnet;

/// <summary>
/// Helper function to pad a string to a specified length.
/// </summary>
/// <param name="str">The string to be padded.</param>
/// <param name="num">The size of the returned padded string.</param>
/// <param name="paddingChar">The character used for padding (default: space).</param>
/// <returns>THe padded string.</returns>
String CommandsClass::_padTo(String str, const size_t num, const char paddingChar)
{
    String result = str;

    while (num > result.length())
        result += paddingChar;
    
    return result;
}

/// <summary>
/// Helper function to check if the map<String, String> contains the key.
/// </summary>
/// <param name="map">The map to be searched.</param>
/// <param name="key">The key to be searched for.</param>
/// <returns>True if key has been found.</returns>
bool CommandsClass::_contains(std::map<String, String> map, String key)
{
    key.toLowerCase();

    if (auto search = map.find(key); search != map.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
/// Helper function to check if the map<String, BaseCommand> contains the key.
/// </summary>
/// <param name="map">The map to be searched.</param>
/// <param name="key">The key to be searched for.</param>
/// <returns>True if key has been found.</returns>
bool CommandsClass::_contains(std::map<String, BaseCommand> map, String key)
{
    key.toLowerCase();

    if (auto search = map.find(key); search != map.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
/// Helper function to check if the map<String, LongCommand> contains the key.
/// </summary>
/// <param name="map">The map to be searched.</param>
/// <param name="key">The key to be searched for.</param>
/// <returns>True if key has been found.</returns>
bool CommandsClass::_contains(std::map<String, LongCommand> map, String key)
{
    key.toLowerCase();

    if (auto search = map.find(key); search != map.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// @brief Helper function to check if the map<String, FloatCommand> contains the key.
/// @param map The map to be searched.
/// @param key The key to be searched for.
/// @return True if key has been found.
bool CommandsClass::_contains(std::map<String, FloatCommand> map, String key)
{
    key.toLowerCase();

    if (auto search = map.find(key); search != map.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
/// Initializes the Commands class instance specifying callbacks.
/// </summary>
/// <param name="error">The error callback (has a single string parameter).</param>
/// <param name="nop">The nop callback (has no parameters).</param>
void CommandsClass::init(StringCommandCallback error, VoidCommandCallback nop)
{
    _error = error;
    _nop = nop;
}

/// <summary>
/// Adds the command to the associative container for commands with no arguments.
/// </summary>
/// <param name="command">The command to be added.</param>
void CommandsClass::add(BaseCommand command)
{
    if (command.Shortcut.length() > 0)
        _baseShortcuts.insert({command.Shortcut, command.Name});

    _baseCommands.insert({command.Name, command});
}

/// <summary>
/// Adds the command to the associative container for commands with a long argument.
/// </summary>
/// <param name="command">The command to be added.</param>
void CommandsClass::add(LongCommand command)
{
    if (command.Shortcut.length() > 0)
        _numberShortcuts.insert({command.Shortcut, command.Name});

    _longCommands.insert({command.Name, command});
}

/// <summary>
/// Adds the command to the associative container for commands with a float argument.
/// </summary>
/// <param name="command">The command to be added.</param>
void CommandsClass::add(FloatCommand command)
{
    if (command.Shortcut.length() > 0)
        _numberShortcuts.insert({command.Shortcut, command.Name});

    _floatCommands.insert({command.Name, command});
}

/// <summary>
/// Parses the input string and eventually calls the command callback function.
/// </summary>
/// <param name="input">The command line input string.</param>
void CommandsClass::run(String input)
{
    // Check for empty line.
    if (input.length() == 0)
    {
        _nop();

        if (WaitForResponse)
            WaitForResponse = false;

        return;
    }

    // Trim the input line.
    input.trim();
    String temp = input;

    // Remove escaped characters '[A, [B, [C, [D etc.'. Checking backwards.
    for (int index = temp.length() - 2; index >= 0; index--)
    {
        char c = temp[index];

        if (c == '[')
        {
            input.remove(index, 2);
        }
    }

    temp = input;
    temp.toLowerCase();

    // Check if we are waiting for a response
    if (WaitForResponse)
    {
        // If response starts with 'Y' repeat last command.
        if (temp[0] == 'y')
        {
            _lastCommand.func();
            return;
        }

        // Ignore all other inputs.
        WaitForResponse = false;
        return;
    }

    // Remove invalid characters. Checking backwards.
    for (int index = temp.length() - 1; index >= 0; index--)
    {
        char c = temp[index];

        // Allowed characters are digits, alpha, space, dot, plus, minus, and question mark.
        if (!(isAlphaNumeric(c) || isSpace(c) || (c == '.') || (c == '+') || (c == '-') || (c == '?')))
        {
            input.remove(index);
        }
    }

    // Check for arguments.
    String arg0;
    String arg1;
    long integer;
    float number;

    StringSplitter *splitter = new StringSplitter(input, ' ', 3);
    int argCount = splitter->getItemCount();

    // Check for no argument.
    if (argCount == 1)
    {
        arg0 = splitter->getItemAtIndex(0);

        // Check for shortcut.
        if (arg0.length() == 1)
        {
            // If a command shortcut has been found, update the arg0 string.
            if (_contains(_baseShortcuts, arg0))
            {
                arg0 = _baseShortcuts[arg0];
            }
        }

        // If a base command has been found, call callback function.
        if (_contains(_baseCommands, arg0))
        {
            BaseCommand cmd = _baseCommands[arg0];
            _lastCommand = cmd;

            if (cmd.func != nullptr)
                cmd.func();
        }
        // If the command cannot be found call error callback function.
        else
        {
            _error(String("Unknown command '") + input + "' - use help to show available commands");
        }
    }
    // Check for single argument.
    else if (argCount == 2)
    {
        arg0 = splitter->getItemAtIndex(0);
        arg1 = splitter->getItemAtIndex(1);

        // If a command shortcut has been found, update the arg0 string.
        if (_contains(_numberShortcuts, arg0))
        {
            arg0 = _numberShortcuts[arg0];
        }

        // If a long command has been found, check argument and call callback function with valid argument.
        if (_contains(_longCommands, arg0))
        {
            if (!isInteger(arg1))
            {
                _error(String("Provided argument '") + arg1 + "' not a valid integer number");
                return;
            }
            else
            {
                integer = arg1.toInt();
                LongCommand cmd = _longCommands[arg0];
                cmd.Number = integer;

                if (cmd.func != nullptr)
                    cmd.func(cmd.Number);
            }
        }
        // If a float command has been found, check argument and call callback function with valid argument.
        else if (_contains(_floatCommands, arg0))
        {
            if (!isFloat(arg1))
            {
                _error(String("Provided argument '") + arg1 + "' not a valid float number");
                return;
            }
            else
            {
                number = arg1.toFloat();
                FloatCommand cmd = _floatCommands[arg0];
                cmd.Number = number;

                if (cmd.func != nullptr)
                    cmd.func(cmd.Number);
            }
        }
        // If the command cannot be found call error callback function.
        else
        {
            _error(String("Unknown command '") + input + "' - use help to show available commands");
        }
    }
    else
    {
        _error("Only one argument allowed - use help to show available commands");
        return;
    }
}

/// <summary>
/// Returns the command help.
/// </summary>
/// <returns>String containing the command help.</returns>
String CommandsClass::help()
{
	String help = String("Yard Control:") + CRLF +
                         "Allows to control the linear actuator providing a set of commands." + CRLF + CRLF +
                         "The following commands with no argument are available:" + CRLF + CRLF;

    int maxNameLength = 0;

    // Find the maximum length of the available base commands (with shortcut).
    for (auto const &shortcut : _baseShortcuts)
    {
        BaseCommand command = _baseCommands[shortcut.second];
        maxNameLength = max(command.Name.length(), maxNameLength);
    }

    // Generate the help string for the available base commands (with shortcut).
    for (auto const &shortcut : _baseShortcuts)
    {
        BaseCommand command = _baseCommands[shortcut.second];
        String padded = _padTo(command.Name, maxNameLength);
        help += String("    ") + command.Shortcut + " | " + padded + " - " + command.Description + CRLF;
    }

    help += CRLF;

    maxNameLength = 0;

    // Find the maximum length of the available base commands (no shortcut).
    for (auto const &command : _baseCommands)
    {
        if (command.second.Shortcut.length() == 0)
        {
            maxNameLength = max(command.second.Name.length(), maxNameLength);
        }
    }

    // Generate the help string for the available base commands (no shortcut).
    for (auto const &command : _baseCommands)
    {
        if (command.second.Shortcut.length() == 0)
        {
            String padded = _padTo(command.second.Name, maxNameLength);
            help += String("    ") + padded + " - " + command.second.Description + CRLF;
        }
    }

    help += CRLF;
    help += String("The following commands require an argument:") + CRLF + CRLF;

    maxNameLength = 0;

    // Find the maximum length of the available integer commands (with shortcut).
    for (auto const &command : _longCommands)
    {
        if (command.second.Shortcut.length() > 0)
        {
            maxNameLength = max(command.second.Name.length(), maxNameLength);
        }
    }

    // Generate the help string for the available integer commands (with shortcut).
    for (auto const &command : _longCommands)
    {
        if (command.second.Shortcut.length() > 0)
        {
            String padded = _padTo(command.second.Name, maxNameLength);
            help += String("    ") + command.second.Shortcut + " | " + padded + " <integer> - " + command.second.Description + CRLF;
        }
    }

    maxNameLength = 0;

    // Find the maximum length of the available number commands (with shortcut).
    for (auto const &command : _floatCommands)
    {
        if (command.second.Shortcut.length() > 0)
        {
            maxNameLength = max(command.second.Name.length(), maxNameLength);
        }
    }

    // Generate the help string for the available number commands (with shortcut).
    for (auto const &command : _floatCommands)
    {
        if (command.second.Shortcut.length() > 0)
        {
            String padded = _padTo(command.second.Name, maxNameLength);
            help += String("    ") + command.second.Shortcut + " | " + padded + " <number>  - " + command.second.Description + CRLF;
        }
    }

    help += CRLF;

    maxNameLength = 0;

    // Find the maximum length of the available integer commands (no shortcut).
    for (auto const& command : _longCommands)
    {
        if (command.second.Shortcut.length() == 0)
        {
            maxNameLength = max(command.second.Name.length(), maxNameLength);
        }
    }

    // Generate the help string for the available number commands (no shortcut).
    for (auto const &command : _longCommands)
    {
        if (command.second.Shortcut.length() == 0)
        {
            String padded = _padTo(command.second.Name, maxNameLength);
            help += String("    ") + padded + " <integer> - " + command.second.Description + CRLF;
        }
    }

    // Find the maximum length of the available number commands (no shortcut).
    for (auto const &command : _floatCommands)
    {
        if (command.second.Shortcut.length() == 0)
        {
            maxNameLength = max(command.second.Name.length(), maxNameLength);
        }
    }

    // Generate the help string for the available number commands (no shortcut).
    for (auto const &command : _floatCommands)
    {
        if (command.second.Shortcut.length() == 0)
        {
            String padded = _padTo(command.second.Name, maxNameLength);
            help += String("    ") + padded + " <number>  - " + command.second.Description + CRLF;
        }
    }

    return help + CRLF;
}

/// <summary>
/// Helper function to check for a valid integer number.
/// </summary>
/// <param name="number">The string to be checked.</param>
/// <returns>True if valid.</returns>
boolean CommandsClass::isInteger(String number)
{
    String buffer;
    char first = number.charAt(0);

    if (first == '+' || first == '-')
    {
        buffer = &number[1];
    }
    else
    {
        buffer = number;
    }

    for (int i = 0; i < buffer.length(); i++)
    {
        if (!isDigit(buffer.charAt(i)))
        {
            return false;
        }
    }

    return true;
}

/// <summary>
/// Helper function to check for a valid float number.
/// </summary>
/// <param name="number">The string to be checked.</param>
/// <returns>True if valid.</returns>
boolean CommandsClass::isFloat(String number)
{
    String buffer;
    boolean decimalPoint = false;
    char first = number.charAt(0);

    if (first == '+' || first == '-' || first == '.')
    {
        buffer = &number[1];
    }
    else
    {
        buffer = number;
    }

    for (int i = 0; i < buffer.length(); i++)
    {
        if (buffer.charAt(i) == '.')
        {
            if (decimalPoint)
            {
                return false;
            }
            else
            {
                decimalPoint = true;
            }
        }
        else if (!isDigit(buffer.charAt(i)))
        {
            return false;
        }
    }

    return true;
}