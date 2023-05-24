// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>20-5-2023 7:32 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#pragma region Command Callbacks

void nop();                     // The NOP command (do nothing).
void json();                    // Toggles the output (print) mode.
void quit();                    // Terminate the program.
void help();                    // Displays the help message.
void verbose();                 // Toggles the verbose output mode.
void error(String message);     // Display an error message.

void status();
void position();
void plus();
void minus();
void forward();
void backward();
void calibrate();
void enable();
void disable();
void stop();
void home();
void gpio();

void yard();
void pico();
void wifi();
void server();
void system();
void stepper();
void actuator();
void settings();
void appsettings();
void reboot();
void reset();
void save();
void load();

void smallstep();
void minstep();
void retract();

void rpm();
void speed();
void minspeed();
void maxspeed();
void maxsteps();
void microsteps();

void moveAway();
void moveAbsolute(long value);
void moveRelative(long value);
void moveToTrack(long value);

void moveAbsoluteDistance(float value);
void moveRelativeDistance(float value);

void smallstep(float value);
void minstep(float value);
void retract(float value);

void minspeed(float value);
void maxspeed(float value);
void maxsteps(long value);
void microsteps(long value);

#pragma endregion

// The supported callbacks (void, long, and float).
typedef void (*VoidCommandCallback)();
typedef void (*LongCommandCallback)(long value);
typedef void (*FloatCommandCallback)(float value);

/// <summary>
/// Command supporting shortcuts, a description, and a callback function with no arguments.
/// </summary>
class BaseCommand
{
public:
    BaseCommand() {}
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
///     parse()    - Parses the input line and runs the command.
///     getHelp()  - Gets a printable help string on the available commands.
///
/// The following command types are supported:
///
///     BaseCommand  - A command with an optional shortcut and no arguments.
///     LongCommand  - A command with an optional shortcut and a single long argument.
///     FloatCommand - A command with an optional shortcut and a single float argument.
///
/// </summary>
class CommandsClass
{
private:
    String _padTo(String str, const size_t num, const char paddingChar = ' ');

    static const int MAX_BASE_COMMANDS = 39;
    static const int MAX_LONG_COMMANDS = 5;
    static const int MAX_FLOAT_COMMANDS = 7;

    static const int MAX_BASE_COMMAND_LENGTH = 12;
    static const int MAX_BASE_SHORTCUT_COMMAND_LENGTH = 9;

    static const int MAX_ARG1_COMMAND_LENGTH = 12;
    static const int MAX_ARG1_SHORTCUT_COMMAND_LENGTH = 6;

    /// <summary>
    /// The list of supported base commands (no arguments).
    /// </summary>
    BaseCommand _baseCommands[MAX_BASE_COMMANDS] = {
        { "help",         "?", "Shows this help information.",                 help         },  // 1
        { "quit",         "q", "Terminates the program.",                      quit         },  // 2
        { "json",         "j", "Toggle JSON output mode.",                     json         },  // 3
        { "verbose",      "v", "Toggle verbose output.",                       verbose      },  // 4
                                                                                            
        { "status",       "s", "Shows the current state of the motor driver.", status       },  // 5
        { "position",     "p", "Shows the current position.",                  position     },  // 6
        { "away",         "a", "Retract in the opposite direction.",           moveAway     },  // 7
        { "plus",         "+", "Moves a step forward.",                        plus         },  // 8
        { "minus",        "-", "Moves a step backward.",                       minus        },  // 9
        { "forward",      "f", "Moves a 0.1 mm distance forward.",             forward      },  // 10
        { "backward",     "b", "Moves a 0.1 mm distance backward.",            backward     },  // 11
        { "calibrate",    "c", "Run a calibration sequence.",                  calibrate    },  // 12
        { "enable",       "e", "Enabling the output (after disable).",         enable       },  // 13
        { "disable",      "d", "Stops the motor by disabling the output.",     disable      },  // 14
        { "stop",         "x", "Stops the running motor (decelerating).",      stop         },  // 15
        { "home",         "h", "Moves to home position (position = 0).",       home         },  // 16
        { "gpio",         "g", "Shows the GPIO input and output pin values.",  gpio         },  // 17

        { "yard",         "",  "Show yard track settings.",                    yard         },  // 18
        { "pico",         "",  "Show Pico W pin layout.",                      pico         },  // 19
        { "wifi",         "",  "Shows the WiFi information.",                  wifi         },  // 20
        { "server",       "",  "Shows the server information.",                server       },  // 21
        { "system",       "",  "Shows the system information.",                system       },  // 22
        { "stepper",      "",  "Shows the stepper settings.",                  stepper      },  // 23
        { "actuator",     "",  "Shows the actuator settings.",                 actuator     },  // 24
        { "settings",     "",  "Shows all settings information.",              settings     },  // 25
        { "appsettings",  "",  "Shows the appsettings file.",                  appsettings  },  // 26
        { "reboot",       "",  "Reboots the RP2040.",                          reboot       },  // 27
        { "reset",        "",  "Resets the current position to zero.",         reset        },  // 28
        { "save",         "",  "Saves the updated application settings.",      save         },  // 29
        { "load",         "",  "(Re)loads the application settings.",          load         },  // 30

        { "smallstep",    "",  "Gets the small move distance (mm).",           smallstep    },  // 31
        { "minstep",      "",  "Gets the min move distance (mm).",             minstep      },  // 32
        { "retract",      "",  "Gets the retract distance (mm).",              retract      },  // 33

        { "rpm",          "",  "Gets the speed RPM.",                          rpm          },  // 34
        { "speed",        "",  "Gets the speed (steps per second).",           speed        },  // 35
        { "minspeed",     "",  "Gets the minimum speed (steps per second).",   minspeed     },  // 36
        { "maxspeed",     "",  "Gets the maximum speed (steps per second).",   maxspeed     },  // 37
        { "maxsteps",     "",  "Gets the ramp steps to maximum speed.",        maxsteps     },  // 38
        { "microsteps",   "",  "Gets the microsteps settings.",                microsteps   },  // 39
    };

    int _findBaseCommandByShortcut(String shortcut);
    int _findBaseCommandByName(String name);
    void _processBaseCommand(int index);

    /// <summary>
    /// The list of supported long commands (one long argument).
    /// </summary>
    LongCommand _longCommands[MAX_LONG_COMMANDS] = {
        { "stepto",     "m", "Moves to absolute position (steps).",   moveAbsolute },           // 1
        { "step",       "s", "Moves the number of steps (relative).", moveRelative },           // 2
        { "track",      "t", "Moves to track number.",                moveToTrack  },           // 3

        { "maxsteps",   "",  "Sets the ramp steps to maximum speed.", maxsteps     },           // 5
        { "microsteps", "",  "Sets the microsteps.",                  microsteps   },           // 6
    };

    int _findLongCommandByShortcut(String shortcut);    // Returns the command index (or -1 if not found).
    int _findLongCommandByName(String name);            // Returns the command index (or -1 if not found).
    void _processLongCommand(int index, String arg);    // Process the command at index using the argument.

    /// <summary>
    /// The list of supported float commands (one float argument).
    /// </summary>
    FloatCommand _floatCommands[MAX_FLOAT_COMMANDS] = {
        { "moveto",       "a", "Moves to absolute position (mm).",    moveAbsoluteDistance },   // 1
        { "move",         "r", "Moves the number of mm (relative).",  moveRelativeDistance },   // 2

        { "smallstep",    "",  "Sets the small move distance (mm).",          smallstep    },   // 3
        { "minstep",      "",  "Sets the min move distance (mm).",            minstep      },   // 4
        { "retract",      "",  "Sets the retract distance (mm).",             retract      },   // 5

        { "minspeed",     "",  "Sets the minimum speed (steps per second).",  minspeed     },   // 6
        { "maxspeed",     "",  "Sets the maximum speed (steps per second).",  maxspeed     },   // 7
    };

    int _findFloatCommandByShortcut(String shortcut);   // Returns the command index (or -1 if not found).
    int _findFloatCommandByName(String name);           // Returns the command index (or -1 if not found).
    void _processFloatCommand(int index, String arg);   // Process the command at index using the argument.

    BaseCommand _lastCommand;                           // Last base command (used if waiting for response).

public:
    bool JsonOutput = false;                            // Flag indicating JSON output.
    bool WaitForResponse = false;                       // Flag indicating that a command response is expected.

    void parse(String command);                         // Parses the input line and runs the command.
    String getHelp();                                   // Gets a printable help string on the available commands.

    bool isInteger(String number);                      // Returns true if the string is a valid integer number.
    bool isFloat(String number);                        // Returns true if the string is a valid float number.

    inline bool isValidBaseCommand(String command)  { return _findBaseCommandByName (command) != -1; }
    inline bool isValidLongCommand(String command)  { return _findLongCommandByName (command) != -1; }
    inline bool isValidFloatCommand(String command) { return _findFloatCommandByName(command) != -1; }
};
