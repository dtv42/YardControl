// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Wireless.h" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Wednesday, 15th March 2023 19:36:33</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Saturday, 1st April 2023 06:01:04 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <WiFi.h>

/// <summary>
/// This helper class provides WiFi setup support.
/// </summary>
class Wireless
{
private:
    const int CONNECT_DELAY = 1000;
    const int CONNECT_RETRIES = 20;

public:
    bool connectWiFi(void);
    bool createAP(void);
    void setClock(void);
};
