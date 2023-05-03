// --------------------------------------------------------------------------------------------------------------------
// <copyright file="CommandClass.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>21-4-2023 12:55 PM</created>
// <modified>2-5-2023 6:40 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

// Disable all traces when set to 0.
#define ARDUINOTRACE_ENABLE 1

#include <ArduinoTrace.h>
#include <StringSplitter.h>

#include "Commands.h"

/// <summary>
/// Helper function to pad a string to a specified length.
/// </summary>
/// <param name="str">The string to be padded.</param>
/// <param name="num">The size of the returned padded string.</param>
/// <param name="paddingChar">The character used for padding (default: space).</param>
/// <returns>The padded string.</returns>
String CommandsClass::_padTo(String str, const size_t num, const char paddingChar)
{
    String result = str;

    while (num > result.length())
        result += paddingChar;

    return result;
}

/// <summary>
/// Checks for a BaseCommand by shortcut. Returns the command index (or -1 if not found).
/// </summary>
/// <param name="shortcut">The command shortcut.</param>
/// <returns>The command index.</returns>
int CommandsClass::_findBaseCommandByShortcut(String shortcut)
{
    TRACE(); DUMP(shortcut);

    int index = -1;
    shortcut.toLowerCase();

    for (int i = 0; i < MAX_BASE_COMMANDS; i++)
    {
        if (_baseCommands[i].Shortcut == shortcut)
        {
            index = i;
            break;
        }
    }

    return index;
}

/// <summary>
/// Checks for a BaseCommand by name. Returns the command index (or -1 if not found).
/// </summary>
/// <param name="shortcut">The command name.</param>
/// <returns>The command index.</returns>
int CommandsClass::_findBaseCommandByName(String name)
{
    TRACE(); DUMP(name);

    int index = -1;
    name.toLowerCase();

    for (int i = 0; i < MAX_BASE_COMMANDS; i++)
    {
        if (_baseCommands[i].Name == name)
        {
            index = i;
            break;
        }
    }

    return index;
}

/// <summary>
/// Process the BaseCommand using the command index. Calls the command callback function.
/// </summary>
/// <param name="index">The command index.</param>
void CommandsClass::_processBaseCommand(int index)
{
    TRACE(); DUMP(index);

    BaseCommand cmd = _baseCommands[index];
    _lastCommand = cmd;

    if (cmd.func != nullptr)
        cmd.func();
}

/// <summary>
/// Checks for a LongCommand by shortcut. Returns the command index (or -1 if not found).
/// </summary>
/// <param name="shortcut">The command shortcut.</param>
/// <returns>The command index.</returns>
int CommandsClass::_findLongCommandByShortcut(String shortcut)
{
    TRACE(); DUMP(shortcut);

    int index = -1;
    shortcut.toLowerCase();

    for (int i = 0; i < MAX_LONG_COMMANDS; i++)
    {
        if (_longCommands[i].Shortcut == shortcut)
        {
            index = i;
            break;
        }
    }

    return index;
}

/// <summary>
/// Checks for a LongCommand by name. Returns the command index (or -1 if not found).
/// </summary>
/// <param name="shortcut">The command name.</param>
/// <returns>The command index.</returns>
int CommandsClass::_findLongCommandByName(String name)
{
    TRACE(); DUMP(name);

    int index = -1;
    name.toLowerCase();

    for (int i = 0; i < MAX_LONG_COMMANDS; i++)
    {
        if (_longCommands[i].Name == name)
        {
            index = i;
            break;
        }
    }

    return index;
}

/// <summary>
/// Process the LongCommand using the command index. Calls the command callback function if the argument is valid.
/// </summary>
/// <param name="index">The command index.</param>
/// <param name="arg">The command argument.</param>
void CommandsClass::_processLongCommand(int index, String arg)
{
    TRACE(); DUMP(index); DUMP(arg);

    if (!isInteger(arg))
    {
        error(String("Provided argument '") + arg + "' not a valid integer number");
        return;
    }

    LongCommand cmd = _longCommands[index];
    long value = arg.toInt();
    cmd.Number = value;

    if (cmd.func != nullptr)
        cmd.func(cmd.Number);
}

/// <summary>
/// Checks for a FloatCommand by shortcut. Returns the command index (or -1 if not found).
/// </summary>
/// <param name="shortcut">The command shortcut.</param>
/// <returns>The command index.</returns>
int CommandsClass::_findFloatCommandByShortcut(String shortcut)
{
    TRACE(); DUMP(shortcut);

    int index = -1;
    shortcut.toLowerCase();

    for (int i = 0; i < MAX_FLOAT_COMMANDS; i++)
    {
        if (_floatCommands[i].Shortcut == shortcut)
        {
            index = i;
            break;
        }
    }

    return index;
}

/// <summary>
/// Checks for a FloatCommand by name. Returns the command index (or -1 if not found).
/// </summary>
/// <param name="shortcut">The command name.</param>
/// <returns>The command index.</returns>
int CommandsClass::_findFloatCommandByName(String name)
{
    TRACE(); DUMP(name);

    int index = -1;
    name.toLowerCase();

    for (int i = 0; i < MAX_FLOAT_COMMANDS; i++)
    {
        if (_floatCommands[i].Name == name)
        {
            index = i;
            break;
        }
    }

    return index;
}

/// <summary>
/// Process the FloatCommand using the command index. Calls the command callback function if the argument is valid.
/// </summary>
/// <param name="index">The command index.</param>
/// <param name="arg">The command argument.</param>
void CommandsClass::_processFloatCommand(int index, String arg)
{
    TRACE(); DUMP(index); DUMP(arg);

    if (!isFloat(arg))
    {
        error(String("Provided argument '") + arg + "' not a valid float number");
        return;
    }

    float value = arg.toFloat();
    FloatCommand cmd = _floatCommands[index];
    cmd.Number = value;

    if (cmd.func != nullptr)
        cmd.func(cmd.Number);
}

/// <summary>
/// The command parser checks for valid shortcut, command name, and argument.
/// The command input string is trimmed and only valid characters are used.
/// </summary>
/// <param name="command"></param>
void CommandsClass::parse(String command)
{
    TRACE(); DUMP(command);

    // Check for empty input.
    if (command.length() == 0)
    {
        nop();

        if (WaitForResponse)
            WaitForResponse = false;

        return;
    }

    // Trim the input command.
    command.trim();
    String temp = command;

    // Remove escaped characters '[A, [B, [C, [D etc.'. Checking backwards.
    for (int index = temp.length() - 2; index >= 0; index--)
    {
        char c = temp[index];

        if (c == '[')
        {
            command.remove(index, 2);
        }
    }

    temp = command;
    temp.toLowerCase();

    // Remove invalid characters. Checking backwards.
    for (int index = temp.length() - 1; index >= 0; index--)
    {
        char c = temp[index];

        // Allowed characters are digits, alpha, space, dot, plus, minus, and question mark.
        if (!(isAlphaNumeric(c) || isSpace(c) || (c == '.') || (c == '+') || (c == '-') || (c == '?')))
        {
            command.remove(index);
        }
    }

    // Check for empty input.
    if (command.length() == 0)
    {
        nop();

        if (WaitForResponse)
            WaitForResponse = false;

        return;
    }

    // Check if we are waiting for a response
    if (WaitForResponse)
    {
        // If response starts with 'Y' call last command callback.
        if (command[0] == 'y')
        {
            _lastCommand.func();
            return;
        }

        // Ignore all other inputs.
        WaitForResponse = false;
        return;
    }

    // Check for arguments.
    String arg0;
    String arg1;

    StringSplitter* splitter = new StringSplitter(command, ' ', 3);
    int argCount = splitter->getItemCount();

    // Process no argument command.
    if (argCount == 1)
    {
        BaseCommand cmd;

        arg0 = splitter->getItemAtIndex(0);

        // Check for shortcut.
        if (arg0.length() == 1)
        {
            int index = _findBaseCommandByShortcut(arg0);

            if (index != -1)
            {
                // If a command shortcut has been found, process the command.
                _processBaseCommand(index);
            }
            else
            {
                // If the command cannot be found call error callback function.
                error(String("Unknown shortcut '") + command + "' - use help to show available commands");
            }
        }
        else
        {
            int index = _findBaseCommandByName(arg0);

            if (index != -1)
            {
                // If a base command has been found, call callback function.
                _processBaseCommand(index);
            }
            else
            {
                int index = _findLongCommandByName(arg0);

                if (index != -1)
                {
                    // If a long command has been found, display error message.
                    error(String("Command '") + command + "' expects a single (integer) argument");
                }
                else
                {
                    int index = _findFloatCommandByName(arg0);

                    if (index != -1)
                    {
                        // If a float command has been found, display error message.
                        error(String("Command '") + command + "' expects a single (float) argument");
                    }
                    else
                    {
                        // If the command cannot be found call error callback function.
                        error(String("Unknown command '") + command + "' - use help to show available commands");
                    }
                }
            }
        }
    }
    // Process command with single argument.
    else if (argCount == 2)
    {
        arg0 = splitter->getItemAtIndex(0);
        arg1 = splitter->getItemAtIndex(1);

        // Check for shortcut.
        if (arg0.length() == 1)
        {
            int index = _findLongCommandByShortcut(arg0);

            if (index != -1)
            {
                // If a long command shortcut has been found, process the command.
                _processLongCommand(index, arg1);
            }
            else
            {
                index = _findFloatCommandByShortcut(arg0);

                if (index != -1)
                {
                    // If a float command shortcut has been found, process the command.
                    _processFloatCommand(index, arg1);
                }
                else
                {
                    error(String("Unknown shortcut '") + command + "' - use help to show available commands");
                }
            }
        }
        else
        {
            int index = _findLongCommandByName(arg0);

            if (index != -1)
            {
                // If a long command has been found, call callback function.
                _processLongCommand(index, arg1);
            }
            else
            {
                index = _findFloatCommandByName(arg0);

                if (index != -1)
                {
                    // If a float command has been found, call callback function.
                    _processFloatCommand(index, arg1);
                }
                else
                {
                    // If the command cannot be found call error callback function.
                    error(String("Unknown command '") + command + "' - use help to show available commands");
                }
            }
        }
    }
    else
    {
        error("Only one argument allowed - use help to show available commands");
        return;
    }
}

/// <summary>
/// Returns the command help.
/// </summary>
/// <returns>String containing the command help.</returns>
String CommandsClass::getHelp()
{
    String help = String("Yard Control:") + "\r\n" +
        "A fiddle yard controller using a linear actuator." + "\r\n" + "\r\n" +
        "The following commands with no argument are available:" + "\r\n" + "\r\n";

    // Generate the help string for the available base commands (with shortcut).
    for (int i = 0; i < MAX_BASE_COMMANDS; ++i)
    {
        BaseCommand command = _baseCommands[i];

        if (command.Shortcut.length() > 0)
        {
            String padded = _padTo(command.Name, MAX_BASE_SHORTCUT_COMMAND_LENGTH);
            help += String("    ") + command.Shortcut + " | " + padded + " - " + command.Description + "\r\n";
        }
    }

    help += "\r\n";

    // Generate the help string for the available base commands (no shortcut).
    for (int i = 0; i < MAX_BASE_COMMANDS; ++i)
    {
        BaseCommand command = _baseCommands[i];

        if (command.Shortcut.length() == 0)
        {
            String padded = _padTo(command.Name, MAX_BASE_COMMAND_LENGTH);
            help += String("    ") + padded + " - " + command.Description + "\r\n";
        }
    }

    help += "\r\n";
    help += String("The following commands require an argument:") + "\r\n" + "\r\n";

    // Generate the help string for the available integer commands (with shortcut).
    for (int i = 0; i < MAX_LONG_COMMANDS; ++i)
    {
        LongCommand command = _longCommands[i];

        if (command.Shortcut.length() > 0)
        {
            String padded = _padTo(command.Name, MAX_ARG1_SHORTCUT_COMMAND_LENGTH);
            help += String("    ") + command.Shortcut + " | " + padded + " <integer> - " + command.Description + "\r\n";
        }
    }

    help += "\r\n";

    // Generate the help string for the available number commands (with shortcut).
    for (int i = 0; i < MAX_FLOAT_COMMANDS; ++i)
    {
        FloatCommand command = _floatCommands[i];

        if (command.Shortcut.length() > 0)
        {
            String padded = _padTo(command.Name, MAX_ARG1_SHORTCUT_COMMAND_LENGTH);
            help += String("    ") + command.Shortcut + " | " + padded + " <number>  - " + command.Description + "\r\n";
        }
    }

    help += "\r\n";

    // Generate the help string for the available number commands (no shortcut).
    for (int i = 0; i < MAX_LONG_COMMANDS; ++i)
    {
        LongCommand command = _longCommands[i];

        if (command.Shortcut.length() == 0)
        {
            String padded = _padTo(command.Name, MAX_ARG1_COMMAND_LENGTH);
            help += String("    ") + padded + " <integer> - " + command.Description + "\r\n";
        }
    }

    help += "\r\n";

    // Generate the help string for the available number commands (no shortcut).
    for (int i = 0; i < MAX_FLOAT_COMMANDS; ++i)
    {
        FloatCommand command = _floatCommands[i];

        if (command.Shortcut.length() == 0)
        {
            String padded = _padTo(command.Name, MAX_ARG1_COMMAND_LENGTH);
            help += String("    ") + padded + " <number>  - " + command.Description + "\r\n";
        }
    }

    return help;
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