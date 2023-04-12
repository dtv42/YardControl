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