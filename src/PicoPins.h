// --------------------------------------------------------------------------------------------------------------------
// <copyright file="PicoPins.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>12-4-2023 3:07 PM</created>
// <modified>12-4-2023 3:15 PM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include <functional>
#include <map>

#define UNKNOWN_MODE    "[ UNKNOWN      ]"  // String for unknown GPIO mode - getMode() is not available.
#define UNKNOWN_STATUS  "[ UNKNOWN ]"       // String for unknown GPIO status.
#define NO_GPIO_MODE    "[              ]"  // Empty string for GPIO mode if pin is not a GPIO pin.
#define NO_GPIO_STATUS  "[         ]"       // Empty string for GPIO status if pin is not a GPIO pin.
#define NO_GPIO_PIN     255                 // Value indicating an invalid pin number.

#define PICO_PIN_ROWS   20                  // The number of pin rows for the Raspberry Pi Pico W.
#define PICO_W_MAX_PINS 40                  // The total number of pins for the Raspberry Pi Pico W.
#define PICO_W_MAX_GPIO 23                  // The total number of GPIO pins for the Raspberry Pi Pico W (GPIO 26-28 are reserved for ADC).
#define PICO_W_MAX_LEFT 15                  // The total number of pins on the left for the Raspberry Pi Pico W.


/// <summary>
/// A printable string representation of the Raspberry Pi Pico W pins (from Raspberry Pi Pico W Datasheet).
/// </summary>
const String PICO_W_GPIO = String() + "[UART0 TX][I2C0 SDA][SPIO0 RX ][   GP0  ] [01] +--|   |--+ [40] [  VBUS  ]                               " + "\r\n" +
"[UART0 RX][I2C0 SCL][SPIO0 CSn][   GP1  ] [02] |  |___|  | [39] [  VSYS  ]                               " + "\r\n" +
"                               [   GND  ] [03] |         | [38] [  GND   ]                               " + "\r\n" +
"          [I2C1 SDA][SPIO0 SCK][   GP2  ] [04] |         | [37] [ 3V3_EN ]                               " + "\r\n" +
"          [I2C1 SCL][SPIO0 TX ][   GP3  ] [05] |         | [36] [3V3(OUT)]                               " + "\r\n" +
"[UART1 TX][I2C0 SDA][SPIO0 RX ][   GP4  ] [06] |         | [35]           [ADC_VREF ]                    " + "\r\n" +
"[UART1 RX][I2C0 SCL][SPIO0 CSn][   GP5  ] [07] |         | [34] [  GP28  ][  ADC2   ]                    " + "\r\n" +
"                               [   GND  ] [08] |         | [33] [  GND   ][  AGND   ]                    " + "\r\n" +
"          [I2C1 SDA][SPIO0 SCK][   GP6  ] [09] |         | [32] [  GP27  ][  ADC1   ][I2C1 SCL]          " + "\r\n" +
"          [I2C1 SCL][SPIO0 TX ][   GP7  ] [10] |         | [31] [  GP26  ][  ADC0   ][I2C1 SDA]          " + "\r\n" +
"[UART1 TX][I2C0 SDA][SPIO1 RX ][   GP8  ] [11] |         | [30] [  RUN   ]                               " + "\r\n" +
"[UART1 RX][I2C0 SCL][SPIO1 CSn][   GP9  ] [12] |         | [29] [  GP22  ]                               " + "\r\n" +
"                               [   GND  ] [13] |         | [28] [  GND   ]                               " + "\r\n" +
"          [I2C1 SDA][SPIO1 SCK][   GP10 ] [14] |         | [27] [  GP21  ]           [I2C0 SCL]          " + "\r\n" +
"          [I2C1 SCL][SPIO1 TX ][   GP11 ] [15] |         | [26] [  GP20  ]           [I2C0 SDA]          " + "\r\n" +
"[UART0 TX][I2C0 SDA][SPIO1 RX ][   GP12 ] [16] |         | [25] [  GP19  ][SPIO0 TX ][I2C1 SCL]          " + "\r\n" +
"[UART0 RX][I2C0 SCL][SPIO1 CSn][   GP13 ] [17] |         | [24] [  GP18  ][SPIO0 SCK][I2C1 SDA]          " + "\r\n" +
"                               [   GND  ] [18] |         | [23] [  GND   ]                               " + "\r\n" +
"          [I2C1 SDA][SPIO1 SCK][   GP14 ] [19] |         | [22] [  GP17  ][SPIO0 CSn][I2C0 SCL][UART0 RX]" + "\r\n" +
"          [I2C1 SCL][SPIO1 TX ][   GP15 ] [20] |_________| [21] [  GP16  ][SPIO0 RX ][I2C0 SDA][UART0 TX]" + "\r\n";

/// <summary>
/// A string representation of the Raspberry Pi Pico W pin rows (from Raspberry Pi Pico W Datasheet).
/// </summary>
const String PICO_W_PINS[PICO_PIN_ROWS] = {
    String("[  GP0   ] [01] +--|   |--+ [40] [  VBUS  ]"),
    String("[  GP1   ] [02] |  |___|  | [39] [  VSYS  ]"),
    String("[  GND   ] [03] |         | [38] [  GND   ]"),
    String("[  GP2   ] [04] |         | [37] [ 3V3_EN ]"),
    String("[  GP3   ] [05] |         | [36] [3V3(OUT)]"),
    String("[  GP4   ] [06] |         | [35] [ADC_VREF]"),
    String("[  GP5   ] [07] |         | [34] [  ADC2  ]"),
    String("[  GND   ] [08] |         | [33] [  GND   ]"),
    String("[  GP6   ] [09] |         | [32] [  ADC1  ]"),
    String("[  GP7   ] [10] |         | [31] [  ADC0  ]"),
    String("[  GP8   ] [11] |         | [30] [  RUN   ]"),
    String("[  GP9   ] [12] |         | [29] [  GP22  ]"),
    String("[  GND   ] [13] |         | [28] [  GND   ]"),
    String("[  GP10  ] [14] |         | [27] [  GP21  ]"),
    String("[  GP11  ] [15] |         | [26] [  GP20  ]"),
    String("[  GP12  ] [16] |         | [25] [  GP19  ]"),
    String("[  GP13  ] [17] |         | [24] [  GP18  ]"),
    String("[  GND   ] [18] |         | [23] [  GND   ]"),
    String("[  GP14  ] [19] |         | [22] [  GP17  ]"),
    String("[  GP15  ] [20] |_________| [21] [  GP16  ]"),
};

/// <summary>
/// Array of GPIO number for every pin row number. NO_GPIO_PIN (255) indicates no GPIO available at that HW pin.
/// </summary>
const std::pair<uint8_t, uint8_t> GPIO_ROWS[PICO_PIN_ROWS] = {
    {           0, NO_GPIO_PIN },      // [  GP0   ] [01] +--|   |--+ [40] [  VBUS  ]
    {           1, NO_GPIO_PIN },      // [  GP1   ] [02] |  |___|  | [39] [  VSYS  ]
    { NO_GPIO_PIN, NO_GPIO_PIN },      // [  GND   ] [03] |         | [38] [  GND   ]
    {           2, NO_GPIO_PIN },      // [  GP2   ] [04] |         | [37] [ 3V3_EN ]
    {           3, NO_GPIO_PIN },      // [  GP3   ] [05] |         | [36] [3V3(OUT)]
    {           4, NO_GPIO_PIN },      // [  GP4   ] [06] |         | [35] [ADC_VREF]
    {           5, NO_GPIO_PIN },      // [  GP5   ] [07] |         | [34] [  ADC2  ]
    { NO_GPIO_PIN, NO_GPIO_PIN },      // [  GND   ] [08] |         | [33] [  GND   ]
    {           6, NO_GPIO_PIN },      // [  GP6   ] [09] |         | [32] [  ADC1  ]
    {           7, NO_GPIO_PIN },      // [  GP7   ] [10] |         | [31] [  ADC0  ]
    {           8, NO_GPIO_PIN },      // [  GP8   ] [11] |         | [30] [  RUN   ]
    {           9,          22 },      // [  GP9   ] [12] |         | [29] [  GP22  ]
    { NO_GPIO_PIN, NO_GPIO_PIN },      // [  GND   ] [13] |         | [28] [  GND   ]
    {          10,          21 },      // [  GP10  ] [14] |         | [27] [  GP21  ]
    {          11,          20 },      // [  GP11  ] [15] |         | [26] [  GP20  ]
    {          12,          19 },      // [  GP12  ] [16] |         | [25] [  GP19  ]
    {          13,          18 },      // [  GP13  ] [17] |         | [24] [  GP18  ]
    { NO_GPIO_PIN, NO_GPIO_PIN },      // [  GND   ] [18] |         | [23] [  GND   ]
    {          14,          17 },      // [  GP14  ] [19] |         | [22] [  GP17  ]
    {          15,          16 },      // [  GP15  ] [20] |_________| [21] [  GP16  ]
};

/// <summary>
/// This class holds Pico W GPIO pin data.
/// </summary>
class GpioPin
{
private:
    StaticJsonDocument<96> _doc;                            // The Json document representing the pin data.

public:
    GpioPin() {}
    GpioPin(uint8_t pin, PinMode mode, String name = "");   // Initializes the public fields.

    uint8_t Pin;                                            // GPIO Pin (not the actual Pico W pin number) range(PICO_W_MAX_GPIO - 1).
    String Name;                                            // GPIO Pin name (default: GP0 - GP22).
    PinMode Mode;                                           // The pin mode (as defined in Common.h).

    JsonObject toJson();                                    // Get a JSON representation.
};

/// <summary>
/// This class holds selected GPIO pin data for the Raspberry Pi Pico W.
/// </summary>
class GpioPins
{
private:
    StaticJsonDocument<3072> _doc;                                              // The Json document representing all pin data.
    std::map<uint8_t, GpioPin> _pins;                                           // List of all (registered) GPIO pins.

    bool   _contains(uint8_t key);                                              // Helper functionto check if the key (pin) has already been used.
    String _modeToString(PinMode mode);                                         // Helper function to return a string representation.
    String _statusToString(PinStatus status);                                   // Helper function to return a string representation.
    String _padTo(String str, const size_t num, const char paddingChar = ' ');  // Helper to pad the string with a character.

public:
    inline static bool isValid(uint8_t pin) { return ((pin >= 0) && (pin < PICO_W_MAX_GPIO)); }

    void add(uint8_t pin, PinMode mode, String name = "");                      // Adds (registers) a GPIO pin to the list.

    uint8_t getPin(String name);                                                // Returns the GPIO pin number by name.
    String toJsonString();                                                      // Get a serialized JSON string.
    String toString();                                                          // Get a string representation.
};
