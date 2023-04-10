// --------------------------------------------------------------------------------------------------------------------
// <copyright file="TelnetServer.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////

#include "TelnetServer.h"

/////////////////////////////////////////////////////////////////

void TelnetServer::handleInput()
{
    char c = client.read();
    // collect string
    if (_lineMode)
    {
        if (c != '\n')
        {
            if (c >= 32 && c < 127)
            {
                input += c;
            }
            // Respond to backspace.
            if (c == 8)
            {
                input.remove(input.length() - 1);
                client.write(' ');
                client.write(8);
            }
            // EOL -> send input
        }
        else
        {
            on_input(input);
            input = "";
        }
        // send individual characters
    }
    else
    {
        if (input.length())
        {
            on_input(input + String(c));
            input = "";
        }
        else
        {
            on_input(String(c));
        }
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::print(const char c)
{
    if (client && isConnected())
    {
        client.print(c);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::print(const String &str)
{
    if (client && isConnected())
    {
        client.print(str);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println(const String &str)
{
    if (client && isConnected())
    {
        client.println(str);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println(const char c)
{
    if (client && isConnected())
    {
        client.println(c);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println()
{
    if (client && isConnected())
    {
        client.println();
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::print(unsigned char b, int base)
{
    if (client && isConnected())
    {
        client.print(b, base);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println(unsigned char b, int base)
{
    if (client && isConnected())
    {
        client.println(b, base);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::print(const Printable &x)
{
    if (client && isConnected())
    {
        client.print(x);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println(const Printable &x)
{
    if (client && isConnected())
    {
        client.println(x);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::print(unsigned int n, int base)
{
    if (client && isConnected())
    {
        client.print(n, base);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println(unsigned int n, int base)
{
    if (client && isConnected())
    {
        client.println(n, base);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::print(int n, int base)
{
    if (client && isConnected())
    {
        client.print(n, base);
    }
}

/////////////////////////////////////////////////////////////////

void TelnetServer::println(int n, int base)
{
    if (client && isConnected())
    {
        client.println(n, base);
    }
}

/////////////////////////////////////////////////////////////////

size_t TelnetServer::printf(const char *format, ...)
{
    int len = 0;
    if (client && isConnected())
    {
        char loc_buf[64];
        char *temp = loc_buf;
        va_list arg;
        va_list copy;
        va_start(arg, format);
        va_copy(copy, arg);
        len = vsnprintf(temp, sizeof(loc_buf), format, copy);
        va_end(copy);
        if (len < 0)
        {
            va_end(arg);
            return 0;
        };
        if (len >= (int)sizeof(loc_buf))
        {
            temp = (char *)malloc(len + 1);
            if (temp == NULL)
            {
                va_end(arg);
                return 0;
            }
            len = vsnprintf(temp, len + 1, format, arg);
        }
        va_end(arg);
        len = client.write((uint8_t *)temp, len);
        if (temp != loc_buf)
        {
            free(temp);
        }
    }
    return len;
}

/////////////////////////////////////////////////////////////////

bool TelnetServer::isLineModeSet()
{
    return _lineMode;
}

/////////////////////////////////////////////////////////////////

void TelnetServer::setLineMode(bool value /* = true */)
{
    _lineMode = value;
}

/////////////////////////////////////////////////////////////////