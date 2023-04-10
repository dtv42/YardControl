// --------------------------------------------------------------------------------------------------------------------
// <copyright file="TelnetStream.h" company="DTV-Online">
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

#pragma once
#ifndef TelnetStream_h
#define TelnetStream_h

/////////////////////////////////////////////////////////////////

#include "TelnetBase.h"

/////////////////////////////////////////////////////////////////

class TelnetStream : public TelnetBase, public Stream
{
public:
    using TelnetBase::TelnetBase;

    int available();
    int read();
    int peek();
    void flush();

    size_t write(uint8_t);

protected:
    void handleInput();
};

/////////////////////////////////////////////////////////////////

// << operator
//  template<class T> inline TelnetStream &operator <<(TelnetStream &obj, T arg) { obj.print(arg); return obj; }

/////////////////////////////////////////////////////////////////

#endif
/////////////////////////////////////////////////////////////////