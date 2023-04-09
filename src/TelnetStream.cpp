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