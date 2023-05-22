// --------------------------------------------------------------------------------------------------------------------
// <copyright file="TelnetStream.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
//  Author: Lennart Hennigs https://www.lennarthennigs.de
//  Copyright(C) 2018 - 2023 Lennart Hennigs.
//  Released under the MIT license.
// --------------------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////

#include "TelnetStream.h"

/////////////////////////////////////////////////////////////////

void TelnetStream::handleInput()
{
    char c = client.read();
    on_input(String(c));
}

/////////////////////////////////////////////////////////////////

int TelnetStream::available()
{
    if (client && isConnected())
    {
        return client.available();
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////

int TelnetStream::read()
{
    if (client && isConnected())
    {
        return client.read();
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////

int TelnetStream::peek()
{
    if (client && isConnected())
    {
        return client.peek();
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////

void TelnetStream::flush()
{
    if (client && isConnected())
    {
        client.flush();
    }
}

/////////////////////////////////////////////////////////////////

size_t TelnetStream::write(uint8_t data)
{
    if (client && isConnected())
    {
        return client.write(data);
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////
