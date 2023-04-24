// --------------------------------------------------------------------------------------------------------------------
// <copyright file="HttpHelper.ino" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>24-4-2023 10:32 AM</created>
// <modified>24-4-2023 10:41 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------

/// <summary>
/// Stream a file to the Http client. The Mime type is set based on the file extension. 
/// </summary>
/// <param name="path">The path of the file.</param>
void sendFile(String path)
{
    String mime = "application/octet-stream";

    if (path.endsWith(".html"))
    {
        mime = "text/html";
    }
    else if (path.endsWith(".css"))
    {
        mime = "text/css";
    }
    else if (path.endsWith(".js"))
    {
        mime = "application/javascript";
    }
    else if (path.endsWith(".json"))
    {
        mime = "application/json";
    }
    else if (path.endsWith(".ico"))
    {
        mime = "image/x-icon";
    }

    if (LittleFS.exists(path))
    {
        File file = LittleFS.open(path, "r");

        if (!file)
        {
            HttpServer.send(500, "text/plain", String("Error opening file ") + path + ".");
        }
        else
        {
            HttpServer.setContentLength(file.size());
            HttpServer.streamFile(file, mime);
        }

        file.close();
    }
    else
    {
        HttpServer.send(404, "text/plain", String("File ") + path + " not found");
    }
}

/// <summary>
/// Recieves a file (string content) and writes to the file system.
/// Note this works for small files only.
/// </summary>
/// <param name="path">The path of the file.</param>
/// <param name="content">The file content (string).</param>
void receiveFile(String path, String content)
{
    File file = LittleFS.open(path, "w");

    if (!file)
    {
        HttpServer.send(500, "text/plain", String("Error opening file ") + path + ".");
    }
    else
    {
        if (file.print(content))
        {
            HttpServer.send(200, "text/plain", "OK");
        }
        else
        {
            HttpServer.send(500, "text/plain", String("Saving file to ") + path + " failed.");
        }
    }

    file.close();
}

/// <summary>
/// Return the default file ('index.html').
/// </summary>
void getRoot()
{
    if (HttpServer.method() != HTTP_GET)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        sendFile("/www/index.html");
    }
}

/// <summary>
/// Returns requested file (uri).
/// </summary>
void getFile()
{
    if (HttpServer.method() != HTTP_GET)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        sendFile(String("/www") + HttpServer.uri());
    }
}

/// <summary>
/// Returns the application settings file ('appsettings.json').
/// </summary>
void getAppSettings()
{
    sendFile(HttpServer.uri());
}

/// <summary>
/// Returns a JSON representation of various info items.
/// </summary>
void getInfo()
{
    if (HttpServer.method() != HTTP_GET)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        String json;
        String info = HttpServer.uri().substring(1);

        if (info == "settings")
        {
            json = Settings.toJsonString();
        }
        else if (info == "system")
        {
            SystemInfo systemInfo;
            json = systemInfo.toJsonString();
        }
        else if (info == "server")
        {
            ServerInfo serverInfo;
            json = serverInfo.toJsonString();
        }
        else if (info == "status")
        {
            ActuatorInfo actuatorInfo;
            json = actuatorInfo.toJsonString();
        }
        else if (info == "wifi")
        {
            WiFiInfo wifiInfo;
            json = wifiInfo.toJsonString();
        }
        else if (info == "gpio")
        {
            json = Pins.toJsonString();
        }
        else
        {
            HttpServer.send(500, "text/plain", "Could not find the info");
            return;
        }

        HttpServer.send(200, "application/json", json);
    }
}

/// <summary>
/// Execute basic command (no arguments). The "reboot" command is executed without waiting for a response.
/// </summary>
void postBaseCommand()
{
    if (HttpServer.method() != HTTP_POST)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        String command = HttpServer.uri().substring(1);

        if (Commands.isValidBaseCommand(command))
        {
            if (command == "reboot")
            {
                Commands.WaitForResponse = true;
            }

            Commands.parse(command);
            HttpServer.send(200, "text/plain", "OK");
        }
        else
        {
            HttpServer.send(404, "text/plain", String("Command ") + command + " not found");
        }
    }
}

/// <summary>
/// Execute integer command (one integer argument).
/// </summary>
void putIntegerCommand()
{
    if (HttpServer.method() != HTTP_PUT)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        int n = HttpServer.args();

        if (n == 0)
        {
            HttpServer.send(400, "text/plain", "A single argument expected");
        }
        if (n > 1)
        {
            HttpServer.send(400, "text/plain", "Only one argument expected");
        }
        else
        {
            String arg = HttpServer.arg(0);

            if (Commands.isInteger(arg))
            {
                String command = HttpServer.uri().substring(1);

                if (Commands.isValidLongCommand(command))
                {
                    command += " " + arg;
                    Commands.parse(command);
                    HttpServer.send(200, "text/plain", "OK");
                }
                else
                {
                    HttpServer.send(404, "text/plain", String("Command ") + command + " not found");
                }
            }
            else
            {
                HttpServer.send(400, "text/plain", String("Argument ") + arg + " not a valid integer");
            }
        }
    }
}

/// <summary>
/// Execute float command (one float argument).
/// </summary>
void putFloatCommand()
{
    if (HttpServer.method() != HTTP_PUT)
    {
        HttpServer.send(405, "text/plain", "Method Not Allowed");
    }
    else
    {
        int n = HttpServer.args();

        if (n == 0)
        {
            HttpServer.send(400, "text/plain", "A single argument expected");
        }
        if (n > 1)
        {
            HttpServer.send(400, "text/plain", "Only one argument expected");
        }
        else
        {
            String arg = HttpServer.arg(0);

            if (Commands.isFloat(arg))
            {
                String command = HttpServer.uri().substring(1);

                if (Commands.isValidFloatCommand(command))
                {
                    command += " " + arg;
                    Commands.parse(command);
                    HttpServer.send(200, "text/plain", "OK");
                }
                else
                {
                    HttpServer.send(404, "text/plain", String("Command ") + command + " not found");
                }
            }
            else
            {
                HttpServer.send(400, "text/plain", String("Argument ") + arg + " not a valid number");
            }
        }
    }
}

/// <summary>
/// Receive the application settings file ('appsettings.json').
/// </summary>
void postSettings()
{
    int n = HttpServer.args();

    if (n != 1)
    {
        HttpServer.send(400, "text/plain", "A file content (JSON) was expected.");
    }
    else
    {
        String arg = HttpServer.arg(0);
        receiveFile(HttpServer.uri(), arg);
    }
}

/// <summary>
/// Reboot the system.
/// </summary>
void postReboot()
{
    // We don't want to wait for a telnet response - rebooting immediately.
    Telnet.print("Rebooting...");
    Telnet.disconnectClient();
    rp2040.reboot();
}

/// <summary>
/// Not found handler (returns the about page).
/// </summary>
void notFound()
{
    sendFile("/www/about.html");
}

