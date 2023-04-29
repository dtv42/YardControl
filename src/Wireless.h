// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Wireless.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>24-4-2023 11:54 AM</modified>
// <author>Peter Trimmel</author>
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
