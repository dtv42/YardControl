// --------------------------------------------------------------------------------------------------------------------
// <copyright file="TelnetServer.h" company="DTV-Online">
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

#pragma once
#ifndef Telnet_h
#define Telnet_h

/////////////////////////////////////////////////////////////////

#include "TelnetBase.h"

/////////////////////////////////////////////////////////////////

class TelnetServer : public TelnetBase
{
public:
    using TelnetBase::TelnetBase;

    void print(const String &str);
    void println(const String &str);
    void print(const char c);
    void println(const char c);
    void print(unsigned char b, int base);
    void println(unsigned char b, int base);
    void print(int n, int base);
    void println(int n, int base);
    void print(unsigned int n, int base);
    void println(unsigned int n, int base);
    void print(const Printable &x);
    void println(const Printable &x);
    void println();
    size_t printf(const char *format, ...);

    bool isLineModeSet();
    void setLineMode(bool value = true);

protected:
    bool _lineMode = true;

    void handleInput();
};

/////////////////////////////////////////////////////////////////

// << operator
template <class T>
inline TelnetServer &operator<<(TelnetServer &obj, T arg)
{
    obj.print(arg);
    return obj;
}

/////////////////////////////////////////////////////////////////

#endif
/////////////////////////////////////////////////////////////////

