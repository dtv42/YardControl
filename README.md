# YardControl

Small project for a 'Shuttle Yard' control using a linear actuator with a stepper motor driver using an Raspberry Pi Pico W.

Supports WiFi providing a Telnet commandline interface, a web interface and REST web api setup for testing the application.

License: [MIT](LICENSE)

## Hardware

### NEMA Stepper Motor
The stepper motor is a typical NEMA 23 motor with the following characteristics:
~~~ txt
      Step Angle:     1.8° (200 steps per revolution)
      Max. PRM:       600
      Phases:         2
      Rotor Inertia:  280 g/cm²
      Mass:           0.72 kg
      Rated Voltage:  3 V - 36 V
      Rated Current:  2.8 A
      Holding Torque: 1.26 Nm
      Detent Torque:  45 mNm
~~~

With higher frequency the usable torque is decreased considerably.


### Stepper Motor Driver
The stepper motor driver used is a TB6600 and allows microstepping up to 16, leading to a frequency of 32 kHz for the maximum RPM. The maximum pulse frequency for a duty cyle of 50/50 is 13 kHz, for a duty cycle of 25/75 it is 20 kHz or 12.5 usec pulse width and a maximum of 375 RPM.

![TB6600](Documents/TB6600.jpg)

It supports speed and direction
control. You can set its micro step and output current with 6 DIP switch. There are 7 kinds
of micro steps (1, 2/A, 2/B, 4, 8, 16) and 8 kinds of current control (0.5A, 1A, 1.5A, 2A, 2.5A, 2.8A, 3.0A, 3.5A) in all. 
All signal terminals adopt high-speed optocoupler isolation.

#### **Features**
- Support 8 kinds of current control
- Support 6 kinds of micro steps adjustable
- The interfaces adopt high-speed optocoupler isolation
- Automatic semi-flow to reduce heat
- Large area heat sink
- Anti-high-frequency interference ability
- Input anti-reverse protection
- Overheat, over current and short circuit

#### **Electrical Specification**

|             |                           |
|-------------|---------------------------|
| Input       | Current 0~5.0A            |
| Output      | Current 0.5-4.0A          |
| Power (MAX) | 160W                      |
| Micro Step  | 1, 2/A, 2/B, 4, 8, 16, 32 |
| Temperature | -10~45 °C                 |
| Humidity    | No Condensation           |
| Weight      | 0.2 kg                    |
| Dimension   | 96*56*33 mm               |

#### **Input / Output**

**Signal Input**
~~~ txt
    PUL+    Pulse +
    PUL-    Pulse - 
    DIR+    Direction +
    DIR-    Direction - 
    EN+     Off-line Control Enable +
    EN-     Off-line Control Enable - 
~~~
**Motor Machine Winding**
~~~ txt
    A+ Stepper motor A+
    A- Stepper motor A-
    B+ Stepper motor B+
    B- Stepper motor B-
~~~
**Power Supply**
~~~ txt
    VCC     VCC (DC9-42V)
    GND     GND
~~~

Note that the ENA input pin of the TB6600 is inverted (enabled = LOW) and is optional (connect the ENA input pin to ground).

### ![Raspberry Pi](Documents/RaspberryPilOGO.png) Pico W
The Raspberry Pi Pico W is a microcontroller board based on the Raspberry Pi RP2040 microcontroller chip
and has been designed to be a low cost yet flexible development platform for RP2040, with a 2.4GHz
wireless interface and the following key features:

- RP2040 microcontroller with 2MB of flash memory
- On-board single-band 2.4GHz wireless interfaces (802.11n)
- Micro USB B port for power and data (and for reprogramming the flash)
- Exposes 26 multi-function 3.3V general purpose I/O (GPIO)
- 23 GPIO are digital-only, with three also being ADC capable
- Can be surface-mounted as a module
- Dual-core cortex M0+ at up to 133MHz
- On-chip PLL allows variable core frequency
- 264kB multi-bank high performance SRAM
- On-board USB1.1 (device or host)
- 30 multi-function general purpose I/O (four can be used for ADC)
- 12-bit 500ksps analogue to digital converter (ADC)

![Pico W](Documents/picow-pinout.png)

### Logic Level Shifter
Since the Raspberry Pi Pico W GPIO support 3.3V and 
the TB6600 motor driver provides 5V inputs, the TXS0108 logic level shifter is used.

![TXS0108](Documents/TXS0108.png)

#### **Features**
- Bi-directional
- Automatic direction control
- Maximum data rates: 110 Mbps (push pull)
- 1.2 Mbps (open drain)
- Low voltage 1.4 to 3.6 V
- High voltage 1.65 to 5.5 V
- Dimensions: 26 mm x 16 mm

# Development

##  ![Arduino](Documents/ArduinoLogo.png) Arduino
Arduino is an open-source hardware and software company, project, and user community that designs and manufactures single-board microcontrollers and microcontroller kits for building digital devices.

The microcontrollers can be programmed using the C and C++ programming languages, using a standard API which is also known as the Arduino Programming Language, inspired by the Processing language and used with a modified version of the Processing IDE.

Support of the Raspberry Pi Pico W is available via the Raspberry Pi Pico Arduino core, Arduino-Pico and can be found at Github (https://github.com/earlephilhower/arduino-pico).

## Additional Libraries
Additional libraries are used. They can be found and installed via the Arduino library manager.

- AccelStepperWithDistance
- AccelStepper
- TelnetLib
- WebServer
- LittelFS
- Blinkenlight
- StringSplitter

## Local Modules
The local modules implement the application settings (read from a JSON file), support for blinking leds, system info, wireless connection, and a command interpreter.

- PicoW.h
- Version.h
- GpioPins.h, GpioPins.cpp
- Actuator.h, Actuator.cpp
- Commands.h, Commands.cpp
- ServerInfo.h, ServerInfo.cpp
- AppSettings.h, AppSettings.cpp
- ActuatorInfo.h, ActuatorInfo.cpp
- SystemInfo.h, SystemInfo.cpp
- WiFiInfo.h, WiFiInfo.cpp
- Wireless.h, Wireless.cpp
- appsettings.json

### YardControl.ino
The main module gets the settings, connects to an wlan access point, creates the commands, the web server and the telnet server.

1. Create (global) instances.
    1. Create the onboard led instance.
    2. Create the actuator instance.
    3. Instanciates the web server (on port 80).
    4. Instanciates the telnet server.
    5. Create the commands instance.
2. Run the setup() function.
   1. Start the Serial interface (USB).
   2. Initialize the LittleFS file system.
   3. Initialize the Settings (appsettings.json).
   4. Initialize the actuator instance.
   5. Connect to a wlan access point.
      1. If not successful create local AP.
      2. If local AP cannot be creates, stop.
   6. Get UTC time using NTP.
   7. Initialize the Commands.
   8. Set up all commands (name, callback).
   9. Setup the HTTP GET requests.
   10. Setup the HTTP POST requests.
   11. Setup the telnet callbacks.
   12. Start the web server.
   13. Start the telnet server.
3.  Enter loop().
    1.  Update Led.
    2.  Update Telnet.
    3.  Update Actuator.
    4.  Update Http Server.

### Commands
This class maintain lists of available commands. A command is a class holding the name, an optional shortcut, and a command function pointer (callback).

The following command types are supported:
~~~ txt
     BaseCommand - A command with an optional shortcut and no arguments.
     LongCommand - A command with an optional shortcut and a single long argument.
     FloatCommand - A command with an optional shortcut and a single float argument.
~~~
Public Fields:
~~~ txt
    JsonOutput      - Flag indicating JSON output (telnet).
    VerboseOutput   - Flag indicating verbose serial output.
    WaitForResponse - Flag indicating that we wait for a (Yes/No) response.
~~~
Public Methods:
~~~ txt
    init()     - Initializes the command list.
    add()      - Creates a new command and adds to the command list.
    run()      - Parses the input line and runs the command.
    help()     - Gets a printable help string on the available commands.
~~~

### InfoClasses
The follwing info classes are implemented

- ServerInfo
- ActuatorInfo
- SystemInfo
- WiFiInfo
- GpioPins

The all provide public fields, initialized in the cunstructor, with various information.

### Settings
All application settings are maintained here. The settings classses provided are all instantiated reading the application settings file 'appsettings.json'.
- Yard Settings
- Actuator Settings
- Stepper Settings
- Server Settings
- Http -Settings
- Telnet Settings
- WiFi Settings
- AP Settings

### appsettings.json
~~~ JSON
{
    "Yard": {
        "Tracks": [
            1600,
            68800,
            121600,
            174400,
            227200,
            280000,
            332800,
            385600,
            438400,
            491200
        ]
    },
    "Actuator": {
        "LedRunning": 3,
        "LedLimit": 4,
        "LedStop": 5,
        "SwitchStop": 6,
        "SwitchLimit1": 7,
        "SwitchLimit2": 8,
        "MoveSpeed": 1000.0,
        "Retract": 2.5
    },
    "Stepper": {
        "PinPUL": 0,
        "PinDIR": 1,
        "PinENA": 2,
        "MicroSteps": 16,
        "MaxSpeed": 2000.0,
        "Acceleration": 300.0,
        "StepsPerRotation": 200,
        "DistancePerRotation": 2.0,
        "MinPulseWidth": 20
    },
    "Http": {
        "Port": 80
    },
    "Telnet": {
        "Port": 23,
        "Prompt": ">"
    },
    "WiFi": {
        "DHCP": true,
        "SSID": "<SSID>",
        "Password": "<Password>",
        "Hostname": "<Hostname>",
        "Address": "<IP-Adress>",
        "Gateway": "<IP-Adress>",
        "Subnet": "255.255.255.0",
        "DNS": "<IP-Adress>"
    },
    "AP": {
        "SSID": "YARD_CONTROL",
        "Password": "<Password>",
        "Hostname": "<Hostname>"
    }
}
~~~

### Telnet Commands
The TelnetLib has been updated to support the delete key (sending a '*space*' and '*backspace*' when receiveing a '*backspace*').

A set of commands are available at the telnet prompt. The '*help*' command provides a list of all supported commands:

~~~ txt
Yard Control:
Allows to control the linear actuator providing a set of commands.

The following commands with no argument are available:

    + | plus      - Moves a step forward.
    - | minus     - Moves a step backward.
    ? | help      - Shows this help information.
    b | backward  - Moves a 0.1 mm distance backward.
    c | calibrate - Run a calibration sequence.
    d | disable   - Stops the motor by disabling the output.
    e | enable    - Enabling the output (after disable).
    f | forward   - Moves a 0.1 mm distance forward.
    g | gpio      - Shows the GPIO input and output pin values.
    h | home      - Moves to home position (position = 0).
    j | json      - Toggle JSON output.
    p | position  - Shows the current position.
    q | quit      - Terminates the program.
    s | status    - Shows the current state of the motor driver.
    r | release   - Release the stopped motor.
    x | stop      - Stops the running motor.

    acceleration - Gets the acceleration (steps/(sec*sec)).
    maxspeed     - Gets the maximum speed (steps/sec).
    speed        - Gets the current speed (steps/(sec*sec)).
    reset        - Resets the current positio to zero.
    reboot       - Reboots the RP2040.
    server       - Shows the server information.
    settings     - Shows the settings information.
    system       - Shows the system information.
    verbose      - Toggle verbose Serial output.
    wifi         - Shows the WiFi information.

The following commands require an argument:

    s | step   <integer> - Moves the number of steps (relative).
    m | stepto <integer> - Moves to absolute position (steps).
    t | track  <integer> - Moves to track number.
    r | move   <number>  - Moves the number of mm (relative).
    a | moveto <number>  - Moves to absolute position (mm).

    acceleration <number>  - Sets the acceleration (steps/(sec*sec)).
    maxspeed     <number>  - Sets the maximum speed. (steps/sec).
    speed        <number>  - Sets the current speed. (steps/sec).
~~~

### Web Pages
A set of web pages are available to current stepper status, system info and settings. The home page allows to control the stpper motor using the REST api provided.
All HTML pages use `Bootstrap 5` (CSS and JS) files for the display. Those files are provided by https://cdn.jsdelivr.net/npm/bootstrap@5.2.3/dist.
Also an extension to Bootstrap to switch between light and dark theme is used (see https://github.com/coliff/dark-mode-switch).

| Path                            | Description                                               |
|---------------------------------|-----------------------------------------------------------|
| www/index.html                  | Returns the main (default) page.                          |
| www/info.html                   | Displays the system settings (hardware and network info). |
| www/settings.html               | Displays the application settings.                        |
| www/about.html                  | A short description of the application and the REST api.  |
|                                 |                                                           |
| www/favicon.ico                 | File containing the shortcut icons.                       | 
| www/css/dark-mode-switch.css    | Bootstrap light switch support.                           |
| www/js/dark-mode-switch.mini.js | Script for Bootstrap light switch support.                |


The web server is used to implement the web interface for the stepper test app. The REST based API is used by the web pages to request settings, current data, and system information. The GET requests can also be used to integrate the sensors into other applications. All provided REST requests returns data in JSON format.

### REST API's

| GET Request       | Description                                           |
|-------------------|-------------------------------------------------------|
| /appsettings.json | Downloads the content of the *appsettings.json* file. |
| /settings         | Returns the complete application settings.            |
| /system           | Returns a set of system information data.             |
| /server           | The server settings (Http, Telnet).                   |
| /status           | The current stepper state (position, speed etc.).     |
| /wifi             | The status of the wifi connection (SSID, RSSI etc.).  |
| /gpio             | The status of the used GPIO pins                      | 


| POST Request      | Description                                           |
|-------------------|-------------------------------------------------------|
| /appsettings.json | Uploads an *appsettings.json* file.                   |
| /plus	            | Move a single step forward.                           |
| /minus	        | Move a single step backward.                          |
| /forward	        | Move a small step (0.1) forward.                      |
| /backward	        | Move a small step (0.1) backward.                     |
| /calibrate	    | Initiates a calibration routine.                      |
| /enable	        | Enables the stepper motor.                            |
| /disable	        | Disables the stepper motor.                           |
| /home	            | Move ro the home position.                            |
| /stop	            | Stops the stepper motor.                              |
| /release          | Relase the stopped motor.                             |
| /reboot	        | Reboots the machine.                                  |
| /step	            | Move the number of steps (relative).                  |
| /move	            | Move the number of mm (relative).                     |
| /stepto	        | Move to absolute position (steps).                    |
| /moveto	        | Move to absolute position (mm).                       |
| /speed	        | Sets the maximum speed (steps per second).            |
| /maxspeed	        | Sets the maximum speed (steps per second).            |
| /acceleration	    | Sets the acceleration (steps per second per second).  |

### GPIO Mapping
The Raspberry Pi Pico W and the GPIO pins (output from 'pico' command).
~~~ Text
[UART0 TX][I2C0 SDA][SPIO0 RX ][   GP0  ] [01] +--|   |--+ [40] [  VBUS  ]                               
[UART0 RX][I2C0 SCL][SPIO0 CSn][   GP1  ] [02] |  |___|  | [39] [  VSYS  ]                               
                               [   GND  ] [03] |         | [38] [  GND   ]                               
          [I2C1 SDA][SPIO0 SCK][   GP2  ] [04] |         | [37] [ 3V3_EN ]                               
          [I2C1 SCL][SPIO0 TX ][   GP3  ] [05] |         | [36] [3V3(OUT)]                               
[UART1 TX][I2C0 SDA][SPIO0 RX ][   GP4  ] [06] |         | [35]           [ADC_VREF ]                    
[UART1 RX][I2C0 SCL][SPIO0 CSn][   GP5  ] [07] |         | [34] [  GP28  ][  ADC2   ]                    
                               [   GND  ] [08] |         | [33] [  GND   ][  AGND   ]                    
          [I2C1 SDA][SPIO0 SCK][   GP6  ] [09] |         | [32] [  GP27  ][  ADC1   ][I2C1 SCL]          
          [I2C1 SCL][SPIO0 TX ][   GP7  ] [10] |         | [31] [  GP36  ][  ADC2   ][I2C1 SDA]          
[UART1 TX][I2C0 SDA][SPIO1 RX ][   GP8  ] [11] |         | [30] [  RUN   ]                               
[UART1 RX][I2C0 SCL][SPIO1 CSn][   GP9  ] [12] |         | [29] [  GP22  ]                               
                               [   GND  ] [13] |         | [28] [  GND   ]                               
          [I2C1 SDA][SPIO1 SCK][   GP10 ] [14] |         | [27] [  GP21  ]           [I2C0 SCL]          
          [I2C1 SCL][SPIO1 TX ][   GP11 ] [15] |         | [26] [  GP20  ]           [I2C0 SDA]          
[UART0 TX][I2C0 SDA][SPIO1 RX ][   GP12 ] [16] |         | [25] [  GP19  ][SPIO0 TX ][I2C1 SCL]          
[UART0 RX][I2C0 SCL][SPIO1 CSn][   GP13 ] [17] |         | [24] [  GP18  ][SPIO0 SCK][I2C1 SDA]          
                               [   GND  ] [18] |         | [23] [  GND   ]                               
          [I2C1 SDA][SPIO1 SCK][   GP14 ] [19] |         | [22] [  GP17  ][SPIO0 CSn][I2C0 SCL][UART0 RX]
          [I2C1 SCL][SPIO1 TX ][   GP15 ] [20] |_________| [21] [  GP16  ][SPIO0 RX ][I2C0 SDA][UART0 TX]
~~~

#### GPIO Used Pins

~~~
[ OUTPUT ][  PUL  ][   GP0  ] [01] +--|   |--+ [40] [  VBUS  ]
[ OUTPUT ][  DIR  ][   GP1  ] [02] |  |___|  | [39] [  VSYS  ]
                   [   GND  ] [03] |         | [38] [  GND   ]
[ OUTPUT ][  ENA  ][   GP2  ] [04] |         | [37] [ 3V3_EN ]
[ OUTPUT ][ Led 1 ][   GP3  ] [05] |         | [36] [3V3(OUT)]
[ OUTPUT ][ Led 2 ][   GP4  ] [06] |         | [35] [ADC_VREF]          
[ OUTPUT ][ Led 3 ][   GP5  ] [07] |         | [34] [  GP28  ]
                   [   GND  ] [08] |         | [33] [  GND   ]
[ INPUT  ][  SW 1 ][   GP6  ] [09] |         | [32] [  GP27  ]
[ INPUT  ][  SW 2 ][   GP7  ] [10] |         | [31] [  GP36  ]
[ INPUT  ][  SW 3 ][   GP8  ] [11] |         | [30] [  RUN   ]
                   [   GP9  ] [12] |         | [29] [  GP22  ]
                   [   GND  ] [13] |         | [28] [  GND   ]
                   [   GP10 ] [14] |         | [27] [  GP21  ]
                   [   GP11 ] [15] |         | [26] [  GP20  ]
                   [   GP12 ] [16] |         | [25] [  GP19  ]
                   [   GP13 ] [17] |         | [24] [  GP18  ]
                   [   GND  ] [18] |         | [23] [  GND   ]
                   [   GP14 ] [19] |         | [22] [  GP17  ]
                   [   GP15 ] [20] |_________| [21] [  GP16  ]
~~~
