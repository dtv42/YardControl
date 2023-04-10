// --------------------------------------------------------------------------------------------------------------------
// <copyright file="TelnetBase.cpp" company="DTV-Online">
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

#include "TelnetBase.h"

/////////////////////////////////////////////////////////////////

TelnetBase::TelnetBase()
{
    connected = false;
}

/////////////////////////////////////////////////////////////////

bool TelnetBase::begin(uint16_t port /* = 23 */, bool checkConnection /* = true */)
{
    ip = "";
    if (checkConnection)
    {
        // connected to WiFi or is RP2040 in AP mode?
        if (WiFi.status() != WL_CONNECTED && !_isIPSet(WiFi.softAPIP()))
            return false;
    }
    server_port = port;
    server = TCPServer(port);
    server.begin();
    server.setNoDelay(true);
    return true;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::loop()
{
    // is there a new client wating?
    if (server.hasClient())
    {
        // no exisintg connection?
        if (!connected)
        {
            connectClient(server.accept());
        }
        else
        {
            if (!isConnected())
            {
                disconnectClient();
                return;
            }
            TCPClient newClient = server.accept();
            attemptIp = newClient.remoteIP().toString();
            // yes, reconnected
            if (attemptIp == ip)
            {
                disconnectClient(false);
                connectClient(newClient, false);
                if (on_reconnect != NULL)
                    on_reconnect(attemptIp);
                // no, throw error
            }
            else
            {
                if (on_connection_attempt != NULL)
                    on_connection_attempt(attemptIp);
            }
        }
    }
    else
    {
        // frequently check if client is still alive
        if (doKeepAliveCheckNow() && connected && !isConnected())
        {
            disconnectClient();
        }
        // check for input
        if (on_input != NULL && client && client.available())
        {
            handleInput();
        }
    }
    yield();
}

/////////////////////////////////////////////////////////////////

bool TelnetBase::doKeepAliveCheckNow()
{
    long now = millis();
    if (now - last_status_check >= keep_alive_interval)
    {
        last_status_check = now;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::setKeepAliveInterval(int ms)
{
    keep_alive_interval = ms;
}

/////////////////////////////////////////////////////////////////

int TelnetBase::getKeepAliveInterval()
{
    return keep_alive_interval;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::connectClient(WiFiClient c, bool triggerEvent)
{
    client = c;
    ip = client.remoteIP().toString();
    client.setNoDelay(true);
    if (triggerEvent && on_connect != NULL)
        on_connect(ip);
    emptyClientStream();
    connected = true;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::disconnectClient(bool triggerEvent)
{
    emptyClientStream();
    client.stop();
    if (triggerEvent && on_disconnect != NULL)
        on_disconnect(ip);
    ip = "";
    connected = false;
}

/////////////////////////////////////////////////////////////////
// helper method, as ESP32's IPAddress has no isSet() method

bool TelnetBase::_isIPSet(IPAddress ip)
{
#if defined(ARDUINO_ARCH_ESP8266)
    return ip.isSet();
#else
    // this works for ESP32, hopefully for others as well
    return ip.toString() != "0.0.0.0";
#endif
}

/////////////////////////////////////////////////////////////////

void TelnetBase::stop()
{
    server.stop();
}

/////////////////////////////////////////////////////////////////

bool TelnetBase::isConnected()
{
    bool res;
#if defined(ARDUINO_ARCH_ESP8266)
    res = client.status() == ESTABLISHED;
#elif defined(ARDUINO_ARCH_ESP32)
    res = client.connected();
#elif defined(ARDUINO_ARCH_RP2040)
    res = client.connected();
#endif
    return res;
}

/////////////////////////////////////////////////////////////////

String TelnetBase::getIP() const
{
    return ip;
}

/////////////////////////////////////////////////////////////////

String TelnetBase::getLastAttemptIP() const
{
    return attemptIp;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::emptyClientStream()
{
    client.flush();
    delay(50);
    while (client.available())
    {
        client.read();
    }
}

/////////////////////////////////////////////////////////////////

void TelnetBase::onConnect(CallbackFunction f)
{
    on_connect = f;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::onConnectionAttempt(CallbackFunction f)
{
    on_connection_attempt = f;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::onReconnect(CallbackFunction f)
{
    on_reconnect = f;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::onDisconnect(CallbackFunction f)
{
    on_disconnect = f;
}

/////////////////////////////////////////////////////////////////

void TelnetBase::onInputReceived(CallbackFunction f)
{
    on_input = f;
}

/////////////////////////////////////////////////////////////////
