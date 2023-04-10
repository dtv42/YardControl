// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GpioPins.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#define ARDIUNOJSON_TAB "    "

#include <ArduinoJson.h>
#include "PicoW.h"

/// <summary>
/// This class holds GPIO pin data. 
/// Note: the pin mode is initialized in init() since a get PinMode() function is not available.
/// If the pin mode is changed during runtime it will not be reflected here.
/// </summary>
class GpioPin
{
private:
    StaticJsonDocument<96> _doc;                    // The Json document representing the pin data.
    String _pinToString(uint8_t pin);               // Helper function to return a string representation.
    String _modeToString(PinMode mode);             // Helper function to return a string representation.
    String _statusToString(PinStatus status);       // Helper function to return a string representation.

public:
    uint8_t Pin;
    PinMode Mode;
    PinStatus Status;

    void init(uint8_t pin, PinMode mode);           // Initialize all pin fields.
    void update() { Status = digitalRead(Pin); };   // Updates the pin state.

    JsonObject toJson();                            // Get a JSON representation.
    String toString();                              // Get a string representation.
};

/// <summary>
/// This class holds selected GPIO pin data for the Raspberry Pi Pico W.
/// </summary>
class GpioPins
{
private:
    StaticJsonDocument<768> _doc;   // The Json document representing the data.

public:
    static const uint8_t UNUSED_PIN = 29;
    inline static bool isValid(uint8_t pin) { return !((pin < 0) && (pin > 22)); }

    GpioPin PinPUL;                 // Output pin for the TB6600 PUL input (step).
    GpioPin PinDIR;                 // Output pin for the TB6600 DIR input (direction).
    GpioPin PinENA;                 // Output pin for the TB6600 ENA input (enable).

    GpioPin LedRunning;             // Led output pin (running).
    GpioPin LedLimit;              // Led output pin (limit 1 hit).
    GpioPin LedStop;              // Led output pin (limit 2 hit).

    GpioPin SwitchStop;                // The emergency stop switch input pin.
    GpioPin SwitchLimit1;              // The limit 1 switch input pin.
    GpioPin SwitchLimit2;              // The limit 2 switch input pin.

    void init();                    // Initialize all pins.
    void update();                  // Updates all pin states.

    String toJsonString();          // Get a serialized JSON representation.
    String toString();              // Get a string representation.
};
