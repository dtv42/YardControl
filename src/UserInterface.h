// --------------------------------------------------------------------------------------------------------------------
// <copyright file="UserInterface.h" company="DTV-Online">                                                                        
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.                                                        
// </copyright>                                                                                                        
// <license>                                                                                                           
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.                    
// </license>                                                                                                          
// <created>05/23/2023 10:54:22</created>                                                                                                 
// <modified>05/24/2023 09:00:35</modified>                                                                                               
// <author>Peter Trimmel</author>                                                                                      
// <summary>                                                                                                           
//   Helper class to control user input and output using telnet and BT serial.
//   This class also maintaines the verbose output flag.                                                                                                                  
// </summary>                                                                                                          
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Arduino.h>

class UserInterface
{
private:
    static constexpr const uint  BTSTRING = 512;    // The maximum string length for SerialBT output.

	bool _verbose = false;                          // Verbose flag indicates verbose output.

public:
    bool getVerbose();                              // Returns the Verbose flag.
    void toggleVerbose();                           // Toggle the verbose flag.

    void show(const String& str);                   // Prints the string at output (ignores verbose flag).

    void println();                                 // Prints an empty new line (if verbose flag is set).
    void print(const String& str);                  // Prints the string at output (if verbose flag is set).
    void println(const String& str);                // Prints the string at output (if verbose flag is set).

    void run();
};













