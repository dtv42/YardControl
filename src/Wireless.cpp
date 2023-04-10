// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Wireless.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>10-4-2023 10:29 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <String.h>
#include <WiFi.h>

#include "AppSettings.h"
#include "Wireless.h"

/// <summary>
/// Application settings used for WiFi configuration.
/// </summary>
extern AppSettings Settings;

/// <summary>
/// Try to connect to WiFi network using the application settings.
/// </summary>
bool Wireless::connectWiFi(void)
{
    bool ok = false;
    WiFi.mode(WIFI_STA);    
	WiFi.setHostname(Settings.WiFi.Hostname.c_str());

    // Using WiFi passphrase to connect.
    if (Settings.WiFi.Password.length() > 0)
    {
        if (Settings.WiFi.DHCP)
        {
            WiFi.begin(Settings.WiFi.SSID.c_str(), Settings.WiFi.Password.c_str());
        }
        else
        {
            IPAddress address;
            IPAddress gateway;
            IPAddress subnet;
            IPAddress dns;

            bool addressOK = address.fromString(Settings.WiFi.Address);
            bool gatewayOK = gateway.fromString(Settings.WiFi.Gateway);
            bool subnetOK = subnet.fromString(Settings.WiFi.Subnet);
            bool dnsOK = dns.fromString(Settings.WiFi.DNS);

            if (addressOK && dnsOK && gatewayOK && subnetOK)
            {
                WiFi.config(address, dns, gateway, subnet);
            }
            else if (addressOK)
            {
                WiFi.config(address);
            }

            WiFi.begin(Settings.WiFi.SSID.c_str(), Settings.WiFi.Password.c_str());
        }
    }
    // Connect to the open WiFi network.
    else
    {
        if (Settings.WiFi.DHCP)
        {
            WiFi.begin(Settings.WiFi.SSID.c_str());
        }
        else
        {
            IPAddress address;
            IPAddress gateway;
            IPAddress subnet;
            IPAddress dns;

            bool addressOK = address.fromString(Settings.WiFi.Address);
            bool gatewayOK = gateway.fromString(Settings.WiFi.Gateway);
            bool subnetOK = subnet.fromString(Settings.WiFi.Subnet);
            bool dnsOK = dns.fromString(Settings.WiFi.DNS);

            if (addressOK && dnsOK && gatewayOK && subnetOK)
            {
                if (dnsOK)
                {
                    WiFi.config(address, dns, gateway, subnet);
                }
                else
                {
                    WiFi.config(address);
                }

                WiFi.begin(Settings.WiFi.SSID.c_str());
            }
        }
    }

    for (int retries = 0; retries < CONNECT_RETRIES; ++retries)
    {
        Serial.print(".");
        delay(CONNECT_DELAY);
        ok = WiFi.status() == WL_CONNECTED;

        if (ok) break;
    }

    Serial.println();

    return ok;
}

/// <summary>
/// Try to create an WiFi access point using the application settings.
/// </summary>
bool Wireless::createAP(void)
{
    bool ok = false;
    WiFi.mode(WIFI_AP);    
	WiFi.setHostname(Settings.WiFi.Hostname.c_str());

    // Use the unique board ID as the SSID.
    String ssid = String(rp2040.getChipID());

    // Create WiFi access point with passphrase.
    if (Settings.AP.Password.length() > 0)
    {
        ok = WiFi.softAP(Settings.AP.SSID.c_str(), Settings.AP.Password.c_str());
    }
    // Create open WiFi access point. 
    else
    {
        ok = WiFi.softAP(ssid.c_str());
    }

    delay(100);

	return ok;
}

/// <summary>
/// Setting the current time using NTP.
/// </summary>
void Wireless::setClock()
{
    NTP.begin("pool.ntp.org", "time.nist.gov");
    NTP.waitSet();
    time_t now = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    String time = String(asctime(&timeinfo));
    Serial.println(time.substring(0, time.length() - 2));
}