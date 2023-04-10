// --------------------------------------------------------------------------------------------------------------------
// <copyright file="EscapeCodes.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:45 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////

#include "EscapeCodes.h"

/////////////////////////////////////////////////////////////////

String EscapeCodes::cls()
{
    return prefix() + "1J";
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::home()
{
    return prefix() + "H";
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::cursorXY(int x, int y)
{
    return prefix() + String(y) + ";" + String(x) + "H";
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::cursorUp(int x)
{
    return prefixAndNumberAndValue(x, 'A');
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::cursorDown(int x)
{
    return prefixAndNumberAndValue(x, 'B');
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::cursorRight(int x)
{
    return prefixAndNumberAndValue(x, 'C');
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::cursorLeft(int x)
{
    return prefixAndNumberAndValue(x, 'D');
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::setBG(int color)
{
    return setAttribute(color + 40);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::setFG(int color)
{
    return setAttribute(color + 30);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::clearLine()
{
    return prefix() + "2K";
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::clearEoLine()
{
    return prefix() + "K";
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::bold(String str)
{
    return setAttribute(1) + str + setAttribute(22);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::blink(String str)
{
    return setAttribute(5) + str + setAttribute(25);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::italic(String str)
{
    return setAttribute(3) + str + setAttribute(23);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::underline(String str)
{
    return setAttribute(4) + str + setAttribute(24);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::inverse(String str)
{
    return setAttribute(7) + str + setAttribute(27);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::showCursor(bool blink)
{
    return prefix() + "?25" + (blink ? 'h' : 'l');
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::reset()
{
    return prefix() + String('m');
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::prefix()
{
    return String("\033[");
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::prefixAndNumberAndValue(int x, char v)
{
    return prefix() + String(x) + String(v);
}

/////////////////////////////////////////////////////////////////

String EscapeCodes::setAttribute(int a)
{
    return prefixAndNumberAndValue(a, 'm');
}

/////////////////////////////////////////////////////////////////
