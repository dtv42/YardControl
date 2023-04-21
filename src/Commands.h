// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Commands.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>21-4-2023 2:30 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma Region Command Callbacks

void nop();                     // The NOP command /just print the prompt.
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
void release();
void stop();
void home();
void gpio();

void pico();
void wifi();
void server();
void system();
void settings();
void reboot();
void reset();

void speed();
void maxspeed();
void acceleration();

void moveAbsolute(long value);
void moveRelative(long value);
void moveToTrack(long value);

void moveAbsoluteDistance(float value);
void moveRelativeDistance(float value);

void speed(float value);
void maxspeed(float value);
void acceleration(float value);

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
    String _padTo(String str, const size_t num, const char paddingChar = ' ');

    static const int MAX_BASE_COMMANDS = 27;
    static const int MAX_LONG_COMMANDS = 3;
    static const int MAX_FLOAT_COMMANDS = 5;

    static const int MAX_BASE_COMMAND_LENGTH = 12;
    static const int MAX_BASE_SHORTCUT_COMMAND_LENGTH = 9;

    static const int MAX_ARG1_COMMAND_LENGTH = 12;
    static const int MAX_ARG1_SHORTCUT_COMMAND_LENGTH = 6;

    /// <summary>
    /// The list of supported base commands (no arguments).
    /// </summary>
    BaseCommand _baseCommands[MAX_BASE_COMMANDS] = {
        { "help",         "?", "Shows this help information.",                 help         },  // Built-in help command.
        { "quit",         "q", "Terminates the program.",                      quit         },  // Quit command (awaiting confirmation).
        { "json",         "j", "Toggle JSON output mode.",                     json         },  // Output mode command.
        { "verbose",      "v", "Toggle verbose output.",                       verbose      },  // Output verbose command.

        { "status",       "s", "Shows the current state of the motor driver.", status       },
        { "position",     "p", "Shows the current position.",                  position     },
        { "plus",         "+", "Moves a step forward.",                        plus         },
        { "minus",        "-", "Moves a step backward.",                       minus        },
        { "forward",      "f", "Moves a 0.1 mm distance forward.",             forward      },
        { "backward",     "b", "Moves a 0.1 mm distance backward.",            backward     },
        { "calibrate",    "c", "Run a calibration sequence.",                  calibrate    },
        { "enable",       "e", "Enabling the output (after disable).",         enable       },
        { "disable",      "d", "Stops the motor by disabling the output.",     disable      },
        { "release",      "r", "Release the stopped motor.",                   release      },
        { "stop",         "x", "Stops the running motor (decelerating).",      stop         },
        { "home",         "h", "Moves to home position (position = 0).",       home         },
        { "gpio",         "g", "Shows the GPIO input and output pin values.",  gpio         },

        { "pico",         "",  "Show Pico W pin layout.",                      pico         },
        { "wifi",         "",  "Shows the WiFi information.",                  wifi         },
        { "server",       "",  "Shows the server information.",                server       },
        { "system",       "",  "Shows the system information.",                system       },
        { "settings",     "",  "Shows the settings information.",              settings     },
        { "reboot",       "",  "Reboots the RP2040.",                          reboot       },
        { "reset",        "",  "Resets the current position to zero.",         reset        },

        { "speed",        "",  "Gets the current speed (steps/(sec*sec)).",    speed        },
        { "maxspeed",     "",  "Gets the maximum speed (steps/sec).",          maxspeed     },
        { "acceleration", "",  "Gets the acceleration (steps/(sec*sec)).",     acceleration },
    };

    int _findBaseCommandByShortcut(String shortcut);
    int _findBaseCommandByName(String name);
    void _processBaseCommand(int index);

    /// <summary>
    /// The list of supported long commands (one long argument).
    /// </summary>
    LongCommand _longCommands[MAX_LONG_COMMANDS] = {
        { "stepto", "m", "Moves to absolute position (steps).",   moveAbsolute },
        { "step",   "s", "Moves the number of steps (relative).", moveRelative },
        { "track",  "t", "Moves to track number.",                moveToTrack  },
    };

    int _findLongCommandByShortcut(String shortcut);    // Returns the command index (or -1 if not found).
    int _findLongCommandByName(String name);            // Returns the command index (or -1 if not found).
    void _processLongCommand(int index, String arg);    // Process the command at index using the argument.

    /// <summary>
    /// The list of supported float commands (one float argument).
    /// </summary>
    FloatCommand _floatCommands[MAX_FLOAT_COMMANDS] = {
        { "moveto",       "a", "Moves to absolute position (mm).",             moveAbsoluteDistance },
        { "move",         "r", "Moves the number of mm (relative).",           moveRelativeDistance },

        { "speed",        "",  "Sets the current speed. (steps/sec).",         speed },
        { "maxspeed",     "",  "Sets the maximum speed. (steps/sec).",         maxspeed },
        { "acceleration", "",  "Sets the acceleration (steps/(sec*sec)).",     acceleration },
    };

    int _findFloatCommandByShortcut(String shortcut);   // Returns the command index (or -1 if not found).
    int _findFloatCommandByName(String name);           // Returns the command index (or -1 if not found).
    void _processFloatCommand(int index, String arg);   // Process the command at index using the argument.

    BaseCommand _lastCommand;                           // Last base command (used if waiting for response).

public:
    bool JsonOutput = false;                            // Flag indicating JSON output.
    bool VerboseOutput = false;                         // Flag indicating verbose output.
    bool WaitForResponse = false;                       // Flag indicating that a command response is expected.

    void parse(String command);                         // Parses the input line and runs the command.
    String getHelp();                                   // Gets a printable help string on the available commands.

    bool isInteger(String number);                      // Returns true if the string is a valid integer number.
    bool isFloat(String number);                        // Returns true if the string is a valid float number.

    inline bool isValidBaseCommand(String command)  { return _findBaseCommandByName (command) != -1; }
    inline bool isValidLongCommand(String command)  { return _findLongCommandByName (command) != -1; }
    inline bool isValidFloatCommand(String command) { return _findFloatCommandByName(command) != -1; }
};