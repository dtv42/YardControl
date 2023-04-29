// --------------------------------------------------------------------------------------------------------------------
// <copyright file="PicoPins.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>12-4-2023 3:07 PM</created>
// <modified>12-4-2023 3:08 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#include "PicoPins.h"

/// <summary>
/// Initializes the pin fields. A default name is used if no name was specified.
/// The GPIO pin is initialized with the specified pin mode.
/// </summary>
/// <param name="pin">The pin number.</param>
/// <param name="name">The pin name.</param>
/// <param name="mode">The pin mode.</param>
GpioPin::GpioPin(uint8_t pin, PinMode mode, String name)
{
    if ((name == nullptr) || (name.length() == 0))
        name = "   ";

    Pin = pin;
    Name = name;
    Mode = mode;
    pinMode(pin, mode);
}

/// <summary>
/// Creates a JSON object with the current field values. The status is updated.
/// </summary>
/// <returns>The new JsonObject (pointing to the static JSON document).</returns>
JsonObject GpioPin::toJson()
{
    PinStatus status = digitalRead(Pin);

    _doc.clear();
    _doc["Pin"] = Pin;
    _doc["Name"] = Name;
    _doc["Mode"] = Mode;
    _doc["Status"] = status;

    return _doc.as<JsonObject>();
}

/// <summary>
/// Helper function to pad a string to a specified length.
/// </summary>
/// <param name="str">The string to be padded.</param>
/// <param name="num">The size of the returned padded string.</param>
/// <param name="paddingChar">The character used for padding (default: space).</param>
/// <returns>THe padded string.</returns>
String GpioPins::_padTo(String str, const size_t num, const char paddingChar)
{
    String result = str;

    while (num > result.length())
        result += paddingChar;

    return result;
}

/// <summary>
/// Helper function to return a string representation of the pin mode.
/// Note that the returned string is(right) padded to the maximum length.
/// </summary>
/// <param name="pin">The GPIO pin mode.</param>
/// <returns>The string containing the pin mode.</returns>
String GpioPins::_modeToString(PinMode mode)
{
    switch (mode)
    {
    case PinMode::INPUT:                    // 0x0
        return String("[ INPUT        ]");
    case PinMode::OUTPUT:                   // 0x1
        return String("[ OUTPUT       ]");
    case PinMode::INPUT_PULLUP:             // 0x2
        return String("[ INPUT_PULLUP ]");
    case PinMode::INPUT_PULLDOWN:           // 0x3
        return String("[INPUT_PULLDOWN]");
    case PinMode::OUTPUT_2MA:               // 0x4
        return String("[ OUTPUT_2MA   ]");
    case PinMode::OUTPUT_4MA:               // 0x5
        return String("[ OUTPUT_4MA   ]");
    case PinMode::OUTPUT_8MA:               // 0x6
        return String("[ OUTPUT_8MA   ]");
    case PinMode::OUTPUT_12MA:              // 0x7
        return String("[ OUTPUT_12MA  ]");
    default:                                // N/A
        return String(UNKNOWN_MODE);
    }
}

/// <summary>
/// Helper function to return a string representation of the pin status.
/// Note that the returned string is(right) padded to the maximum length.
/// </summary>
/// <param name="pin">The GPIO pin status.</param>
/// <returns>The string containing the pin status.</returns>
String GpioPins::_statusToString(PinStatus status)
{
    switch (status)
    {
    case PinStatus::LOW:            // 0
        return String("[ LOW     ]");
    case PinStatus::HIGH:           // 1
        return String("[ HIGH    ]");
    case PinStatus::CHANGE:         // 2
        return String("[ CHANGE  ]");
    case PinStatus::FALLING:        // 3
        return String("[ FALLING ]");
    case PinStatus::RISING:         // 4
        return String("[ RISING  ]");                     
    default:                        // N/A
        return String(UNKNOWN_STATUS);
    }
}

/// <summary>
/// Helper function to check if the map<String, String> contains the key.
/// </summary>
/// <param name="key">The key to be searched for.</param>
/// <returns>True if key has been found.</returns>
bool GpioPins::_contains(uint8_t key)
{
    if (auto search = _pins.find(key); search != _pins.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// <summary>
///  Initializing all used pins using the application settings.
/// </summary>
void GpioPins::add(uint8_t pin, PinMode mode, String name)
{
    if (GpioPins::isValid(pin) && !_contains(pin))
    {
        GpioPin gpio(pin, mode, name);
        _pins.insert({ pin, gpio });
    }
}

/// <summary>
/// Returns the GPIO pin number by name. If the pin cannot be found return -1;
/// </summary>
/// <param name="name">The GPIO pin name.</param>
/// <returns>The GPIO pin or -1.</returns>
uint8_t GpioPins::getPin(String name)
{
    for (auto const& [key, val] : _pins)
    {
        if (val.Name == name) return key;
    }

    return -1;
}

/// <summary>
/// Returns a (pretty) string representation of the updated JSON document.
/// </summary>
/// <returns>The serialized JSON document.</returns>
String GpioPins::toJsonString()
{
    String json;

    _doc.clear();
    JsonArray array = _doc.to<JsonArray>();

    for (auto const& [key, val] : _pins)
    {
        GpioPin pin = val;
        array.add(pin.toJson());
    }

    serializeJsonPretty(_doc, json);

    return json;
}

/// <summary>
/// Returns a printable string representation of the GPIO pins.
/// </summary>
/// <returns>The printable string.</returns>
String GpioPins::toString()
{
    String result = String("GPIO State:") + "\r\n";
    int length = 0;

    // Find the maximum name length for all (registered) GPIO pins.
    for (auto const& [key, val] : _pins)
    {
        length = max(length, val.Name.length());
    }

    // Loop through pin rows and update the result string.
    for (uint8_t row = 0; row < PICO_PIN_ROWS; row++)
    {
        String status1, status2, mode1, mode2, name1, name2;

        std::pair<uint8_t, uint8_t> pins = GPIO_ROWS[row];

        // Check if the (left) pin has been registered and update status, mode, and name.
        if (pins.first != NO_GPIO_PIN)
        {
            status1 = _statusToString(digitalRead(pins.first));

            if (_contains(pins.first))
            {
                GpioPin pin = _pins[pins.first];
                mode1 = _modeToString(pin.Mode);
                name1 = String("[ ") + _padTo(pin.Name, length) + " ]";
            }
            else
            {
                mode1 = UNKNOWN_MODE;
                name1 = String("[ ") + _padTo("", length) + " ]";
            }
        }
        else
        {
            status1 = NO_GPIO_STATUS;
            mode1   = NO_GPIO_MODE;
            name1   = String("[ ") + _padTo("", length) + " ]";
        }

        // Check if the (right) pin has been registered and update status, mode, and name.
        if (pins.second != NO_GPIO_PIN)
        {
            status2 = _statusToString(digitalRead(pins.second));

            if (_contains(pins.second))
            {
                GpioPin pin = _pins[pins.second];
                mode2 = _modeToString(pin.Mode);
                name2 = String("[ ") + _padTo(pin.Name, length) + " ]";
            }
            else
            {
                mode2 = UNKNOWN_MODE;
                name2 = String("[ ") + _padTo("", length) + " ]";
            }
        }
        else
        {
            status2 = NO_GPIO_STATUS;
            mode2   = NO_GPIO_MODE;
            name2   = String("[ ") + _padTo("", length) + " ]";
        }

        result = result + name1 + status1 + mode1 + PICO_W_PINS[row] + mode2 + status2 + name2 + "\r\n";
    }

    return result;
}