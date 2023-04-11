// --------------------------------------------------------------------------------------------------------------------
// <copyright file="PicoW.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include "Defines.h"

/// <summary>
/// A printable string representation of the Raspberry Pi Pico W pins (from Raspberry Pi Pico W Datasheet).
/// </summary>
const String PICO_W_GPIO = String() + "[UART0 TX][I2C0 SDA][SPIO0 RX ][   GP0  ] [01] +--|   |--+ [40] [  VBUS  ]                               " + CRLF +
                                      "[UART0 RX][I2C0 SCL][SPIO0 CSn][   GP1  ] [02] |  |___|  | [39] [  VSYS  ]                               " + CRLF +
                                      "                               [   GND  ] [03] |         | [38] [  GND   ]                               " + CRLF +
                                      "          [I2C1 SDA][SPIO0 SCK][   GP2  ] [04] |         | [37] [ 3V3_EN ]                               " + CRLF +
                                      "          [I2C1 SCL][SPIO0 TX ][   GP3  ] [05] |         | [36] [3V3(OUT)]                               " + CRLF +
                                      "[UART1 TX][I2C0 SDA][SPIO0 RX ][   GP4  ] [06] |         | [35]           [ADC_VREF ]                    " + CRLF +
                                      "[UART1 RX][I2C0 SCL][SPIO0 CSn][   GP5  ] [07] |         | [34] [  GP28  ][  ADC2   ]                    " + CRLF +
                                      "                               [   GND  ] [08] |         | [33] [  GND   ][  AGND   ]                    " + CRLF +
                                      "          [I2C1 SDA][SPIO0 SCK][   GP6  ] [09] |         | [32] [  GP27  ][  ADC1   ][I2C1 SCL]          " + CRLF +
                                      "          [I2C1 SCL][SPIO0 TX ][   GP7  ] [10] |         | [31] [  GP26  ][  ADC2   ][I2C1 SDA]          " + CRLF +
                                      "[UART1 TX][I2C0 SDA][SPIO1 RX ][   GP8  ] [11] |         | [30] [  RUN   ]                               " + CRLF +
                                      "[UART1 RX][I2C0 SCL][SPIO1 CSn][   GP9  ] [12] |         | [29] [  GP22  ]                               " + CRLF +
                                      "                               [   GND  ] [13] |         | [28] [  GND   ]                               " + CRLF +
                                      "          [I2C1 SDA][SPIO1 SCK][   GP10 ] [14] |         | [27] [  GP21  ]           [I2C0 SCL]          " + CRLF +
                                      "          [I2C1 SCL][SPIO1 TX ][   GP11 ] [15] |         | [26] [  GP20  ]           [I2C0 SDA]          " + CRLF +
                                      "[UART0 TX][I2C0 SDA][SPIO1 RX ][   GP12 ] [16] |         | [25] [  GP19  ][SPIO0 TX ][I2C1 SCL]          " + CRLF +
                                      "[UART0 RX][I2C0 SCL][SPIO1 CSn][   GP13 ] [17] |         | [24] [  GP18  ][SPIO0 SCK][I2C1 SDA]          " + CRLF +
                                      "                               [   GND  ] [18] |         | [23] [  GND   ]                               " + CRLF +
                                      "          [I2C1 SDA][SPIO1 SCK][   GP14 ] [19] |         | [22] [  GP17  ][SPIO0 CSn][I2C0 SCL][UART0 RX]" + CRLF +
                                      "          [I2C1 SCL][SPIO1 TX ][   GP15 ] [20] |_________| [21] [  GP16  ][SPIO0 RX ][I2C0 SDA][UART0 TX]" + CRLF;
