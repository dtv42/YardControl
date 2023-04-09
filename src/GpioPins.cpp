// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GpioPins.cpp" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <version></version>
// <project name="YardControl">
// <created>Tuesday, 4th April 2023 12:03:01</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Saturday, 8th April 2023 06:56:43 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#include "AppSettings.h"
#include "GpioPins.h"

/// <summary>
/// Used for initialization of the Gpio pins info.
/// </summary>
extern AppSettings Settings;

#define CRLF "\r\n"

/// <summary>
/// Helper function to return a string representation of the pin number.
/// </summary>
/// <param name="pin">The GPIO pin number.</param>
/// <returns>The string containing the pin number.</returns>
String GpioPin::_pinToString(uint8_t pin)
{
    switch (pin)
    {
    case ( 0): return "GP 0";
    case ( 1): return "GP 1";
    case ( 2): return "GP 2";
    case ( 3): return "GP 3";
    case ( 4): return "GP 4";
    case ( 5): return "GP 5";
    case ( 6): return "GP 6";
    case ( 7): return "GP 7";
    case ( 8): return "GP 8";
    case ( 9): return "GP 9";
    case (10): return "GP10";
    case (11): return "GP11";
    case (12): return "GP12";
    case (13): return "GP13";
    case (14): return "GP14";
    case (15): return "GP15";
    case (16): return "GP16";
    case (17): return "GP17";
    case (18): return "GP18";
    case (19): return "GP19";
    case (20): return "GP20";
    case (21): return "GP21";
    case (22): return "GP22";
    default:   return "N/A ";
    }
}

/// <summary>
/// Helper function to return a string representation of the pin mode.
/// Note that the returned string is(right) padded to the maximum length.
/// </summary>
/// <param name="pin">The GPIO pin mode.</param>
/// <returns>The string containing the pin mode.</returns>
String GpioPin::_modeToString(PinMode mode)
{
    switch (mode)
    {
    case PinMode::INPUT: // 0x0
        return String("INPUT         ");
    case PinMode::OUTPUT: // 0x1
        return String("OUTPUT        ");
    case PinMode::INPUT_PULLUP: // 0x2
        return String("INPUT_PULLUP  ");
    case PinMode::INPUT_PULLDOWN: // 0x3
        return String("INPUT_PULLDOWN");
    case PinMode::OUTPUT_2MA: // 0x4
        return String("OUTPUT_2MA    ");
    case PinMode::OUTPUT_4MA: // 0x5
        return String("OUTPUT_4MA    ");
    case PinMode::OUTPUT_8MA: // 0x6
        return String("OUTPUT_8MA    ");
    case PinMode::OUTPUT_12MA: // 0x7
        return String("OUTPUT_12MA   ");
    default:
        return String("UNKNOWN       ");
    }
}

/// <summary>
/// Helper function to return a string representation of the pin status.
/// Note that the returned string is(right) padded to the maximum length.
/// </summary>
/// <param name="pin">The GPIO pin status.</param>
/// <returns>The string containing the pin status.</returns>
String GpioPin::_statusToString(PinStatus status)
{
    switch (status)
    {
    case PinStatus::LOW:            // 0
        return String("LOW    ");
    case PinStatus::HIGH:           // 1
        return String("HIGH   ");
    case PinStatus::CHANGE:         // 2
        return String("CHANGE ");
    case PinStatus::FALLING:        // 3
        return String("FALLING");
    case PinStatus::RISING:         // 4
        return String("RISING ");
    default:
        return String("UNKNOWN");
    }
}

/// @brief Initializes the pin info data.

/// <summary>
/// Initializes the pin fields. The pin status is read from the GPIO pin.
/// </summary>
/// <param name="pin">The pin number.</param>
/// <param name="mode">The pin mode.</param>
void GpioPin::init(uint8_t pin, PinMode mode)
{
    Pin = GpioPins::isValid(pin) ? pin : GpioPins::UNUSED_PIN;
    Mode = mode;
    pinMode(pin, mode);
    Status = digitalRead(pin);
}

/// <summary>
/// Creates a JSON object with the current field values. The status is updated.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject GpioPin::toJson()
{
    Status = digitalRead(Pin);

    _doc.clear();
    _doc["Pin"] = _pinToString(Pin);
    _doc["Mode"] = _modeToString(Mode);
    _doc["Status"] = _statusToString(Status);

    return _doc.as<JsonObject>();
}

/// <summary>
/// Returns a printable string representation of the GPIO pin info. The status is updated.
/// </summary>
/// <returns>The printable string.</returns>
String GpioPin::toString()
{
    Status = digitalRead(Pin);
    return _pinToString(Pin) + ", " +  _modeToString(Mode) + ", " + _statusToString(Status) + CRLF;
}

/// <summary>
///  Initializing all used pins using the application settings.
/// </summary>
void GpioPins::init()
{
    PinPUL.init(Settings.Stepper.PinPUL, PinMode::OUTPUT);
    PinDIR.init(Settings.Stepper.PinDIR, PinMode::OUTPUT);
    PinENA.init(Settings.Stepper.PinENA, PinMode::OUTPUT);

    LedRunning.init(Settings.Actuator.LedRunning, PinMode::OUTPUT);
    LedLimit. init(Settings.Actuator.LedLimit,  PinMode::OUTPUT);
    LedStop. init(Settings.Actuator.LedStop,  PinMode::OUTPUT);

    SwitchLimit1.init(Settings.Actuator.SwitchLimit1, PinMode::INPUT_PULLUP);
    SwitchLimit2.init(Settings.Actuator.SwitchLimit2, PinMode::INPUT_PULLUP);
    SwitchStop.init(Settings.Actuator.SwitchStop, PinMode::INPUT_PULLUP);
}

/// <summary>
/// Update all pin states.
/// </summary>
void GpioPins::update()
{
    PinPUL.update();
    PinDIR.update();
    PinENA.update();

    LedRunning.update();
    LedLimit.update();
    LedStop.update();

    SwitchLimit1.update();
    SwitchLimit2.update();
    SwitchStop.update();
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String GpioPins::toJsonString()
{
    String json;

    update();
    _doc.clear();

    _doc["PinPUL"]     = PinPUL.toJson();
    _doc["PinDIR"]     = PinDIR.toJson();
    _doc["PinENA"]     = PinENA.toJson();

    _doc["LedRunning"] = LedRunning.toJson();
    _doc["LedLimit"]  = LedLimit.toJson();
    _doc["LedStop"]  = LedStop.toJson();

    _doc["SwitchLimit1"]  = SwitchLimit1.toJson();
    _doc["SwitchLimit2"]  = SwitchLimit2.toJson();
    _doc["SwitchStop"]    = SwitchStop.toJson();

    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Returns a printable string representation of the GPIO pins.
/// </summary>
/// <returns>The printable string.</returns>
String GpioPins::toString()
{
    update();
    return String("GPIO Pin State:") + CRLF +
           "    PinPUL:     " + PinPUL.toString() +
           "    PinDIR:     " + PinDIR.toString() +
           "    PinENA:     " + PinENA.toString() +
           CRLF +
           "    LedRunning: " + LedRunning.toString() +
           "    LedLimit:  " + LedLimit.toString() +
           "    LedStop:  " + LedStop.toString() +
           CRLF +
           "    SwitchLimit1:  " + SwitchLimit1.toString() +
           "    SwitchLimit2:  " + SwitchLimit2.toString() +
           "    SwitchStop:    " + SwitchStop.toString() +
           CRLF;
}