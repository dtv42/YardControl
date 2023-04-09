// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.h" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Friday, 17th March 2023 09:13:31</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Thursday, 6th April 2023 13:36:55 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <Arduino.h>
#include <functional>
#include <map>

// The supported callbacks (void, long, float, and String).
typedef void(*VoidCommandCallback)();
typedef void (*LongCommandCallback)(long value);
typedef void (*FloatCommandCallback)(float value);
typedef void (*StringCommandCallback)(String value);

/// <summary>
/// Command supporting shortcuts, a description, and a callback function with no arguments.
/// </summary>
class BaseCommand
{
public:
    BaseCommand() {}
    BaseCommand(String name, String description, VoidCommandCallback func = nullptr)
        : BaseCommand(name, "", description, func) {}
    BaseCommand(String name, String shortcut, String description, VoidCommandCallback func = nullptr)
    {
        Name = name;
        Shortcut = shortcut;
        Description = description;
        this->func = func;
    }

    String Name = "";
    String Shortcut = "";
    String Description = "";
    VoidCommandCallback func = nullptr;
};

/// <summary>
/// Command supporting shortcuts, a description, and a callback function with a single long argument.
/// </summary>
class LongCommand
{
public:
    LongCommand() {}
    LongCommand(String name, String description, LongCommandCallback func = nullptr)
        : LongCommand(name, "", description, func) {}
    LongCommand(String name, String shortcut, String description, LongCommandCallback func = nullptr)
    {
        Name = name;
        Shortcut = shortcut;
        Description = description;
        this->func = func;
    }

    long Number;
    String Name = "";
    String Shortcut = "";
    String Description = "";
    LongCommandCallback func = nullptr;
};

/// <summary>
/// Command supporting shortcuts, a description, and a callback function with a single float argument.
/// </summary>
class FloatCommand
{
public:
    FloatCommand() {}
    FloatCommand(String name, String description, FloatCommandCallback func = nullptr)
        : FloatCommand(name, "", description, func) {}
    FloatCommand(String name, String shortcut, String description, FloatCommandCallback func = nullptr)
    {
        Name = name;
        Shortcut = shortcut;
        Description = description;
        this->func = func;
    }

    float Number;
    String Name = "";
    String Shortcut = "";
    String Description = "";
    FloatCommandCallback func = nullptr;
};

/// <summary>
/// This class maintains lists of available commands.
///
///     init()     - Initializes the command list.
///     add()      - Creates a new command and adds to the command list.
///     run()      - Parses the input line and runs the command.
///     help()     - Gets a printable help string on the available commands.
///
/// The following command types are supported:
///
///     BaseCommand - A command with an optional shortcut and no arguments.
///     LongCommand - A command with an optional shortcut and a single float argument.
///     FloatCommand - A command with an optional shortcut and a single float argument.
///
/// </summary>
class CommandsClass
{
private:
    std::map<String, String> _baseShortcuts;                                    // The list of all available base command shortcuts.
    std::map<String, String> _numberShortcuts;                                  // The list of all available number command shortcuts.
    std::map<String, BaseCommand> _baseCommands;                                // The list of all available base commands.
    std::map<String, LongCommand> _longCommands;                                // The list of all available number commands.
    std::map<String, FloatCommand> _floatCommands;                              // The list of all available number commands.

    bool _contains(std::map<String, String> map, String key);                   // Returns true if the shortcut map contains the key.
    bool _contains(std::map<String, BaseCommand> map, String key);              // Returns true if the base command map contains the key.
    bool _contains(std::map<String, LongCommand> map, String key);              // Returns true if the long command map contains the key.
    bool _contains(std::map<String, FloatCommand> map, String key);             // Returns true if the float command map contains the key.

    String _padTo(String str, const size_t num, const char paddingChar = ' ');  // Helper to pad the string with a character.

    BaseCommand _lastCommand;                                                   // Last base command (used if waiting for response).
    StringCommandCallback _error;                                               // Callback for the error display.
    VoidCommandCallback _nop;                                                   // Callback for the nop.

public:
    bool JsonOutput = false;                                                    // Flag indicating JSON output.
    bool WaitForResponse = false;                                               // Flag indicating that we wait for a (Yes/No) response.

    void init(StringCommandCallback error, VoidCommandCallback nop);            // Initializes the command list.
    void add(BaseCommand command);                                              // Creates a new base command and adds to the command list.
    void add(LongCommand command);                                              // Creates a new long command and adds to the command list.
    void add(FloatCommand command);                                             // Creates a new float command and adds to the command list.
    void run(String command);                                                   // Parses the input line and runs the command.
    String help();                                                              // Gets a printable help string on the available commands.

    bool isInteger(String number);                                              // Returns true if the string is a valid integer number.
    bool isFloat(String number);                                                // Returns true if the string is a valid float number.

    inline bool isValidBaseCommand(String command)  { return _contains(_baseCommands,  command); }
    inline bool isValidLongCommand(String command)  { return _contains(_longCommands,  command); }
    inline bool isValidFloatCommand(String command) { return _contains(_floatCommands, command); }
};
