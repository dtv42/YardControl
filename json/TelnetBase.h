/////////////////////////////////////////////////////////////////

#pragma once
#ifndef TelnetBase_h
#define TelnetBase_h

/////////////////////////////////////////////////////////////////

#define ASCII_BELL 7
#define ASCII_BACKSPACE 8
#define ASCII_LF 10
#define ASCII_CR 13
#define KEEP_ALIVE_INTERVAL_MS 1000

/////////////////////////////////////////////////////////////////

#define RP2040_PICO_W

#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_RP2040)
#include <WiFi.h>
#endif

/////////////////////////////////////////////////////////////////

#include "DebugMacros.h"

/////////////////////////////////////////////////////////////////

using TCPClient = WiFiClient;
using TCPServer = WiFiServer;

/////////////////////////////////////////////////////////////////
class TelnetBase
{
    typedef void (*CallbackFunction)(String str);

public:
    TelnetBase();

    bool begin(uint16_t port = 23, bool checkConnection = true);
    void stop();
    void loop();

    bool isConnected();
    void disconnectClient(bool triggerEvent = true);

    void setKeepAliveInterval(int ms);
    int getKeepAliveInterval();

    String getIP() const;
    String getLastAttemptIP() const;

    void onConnect(CallbackFunction f);
    void onConnectionAttempt(CallbackFunction f);
    void onReconnect(CallbackFunction f);
    void onDisconnect(CallbackFunction f);
    void onInputReceived(CallbackFunction f);

protected:
    TCPServer server = TCPServer(23); // must be initalized here
    TCPClient client;
    bool connected = false; // needed because I cannot do "client = NULL"
    String ip = "";
    String attemptIp;
    String input = "";

    uint16_t server_port = 23;
    int keep_alive_interval = KEEP_ALIVE_INTERVAL_MS;
    long last_status_check;

    CallbackFunction on_connect = NULL;
    CallbackFunction on_reconnect = NULL;
    CallbackFunction on_disconnect = NULL;
    CallbackFunction on_connection_attempt = NULL;
    CallbackFunction on_input = NULL;

    void emptyClientStream();
    bool _isIPSet(IPAddress ip);
    virtual void handleInput() = 0;

private:
    void connectClient(TCPClient c, bool triggerEvent = true);
    bool doKeepAliveCheckNow();
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
