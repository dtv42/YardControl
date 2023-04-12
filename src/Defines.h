// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Defines.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>11-4-2023 9:04 AM</created>
// <modified>11-4-2023 12:43 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#if !(defined(ARDUINO_RASPBERRY_PI_PICO_W))
#error For RASPBERRY_PI_PICO_W only
#endif

// Disable all traces when set to 0.
#define ARDUINOTRACE_ENABLE 1

// Define the serial speed for the Serial port.
#define SERIAL_SPEED 115200

// Define the end of line characters (used in Telnet output).
#define CRLF "\r\n"
