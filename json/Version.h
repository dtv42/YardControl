// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Version.h" company="DTV-Online">
//   Copyright(c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <project name="YardControl">
// <created>Friday, 7th April 2023 10:31:11</created>
// <author>Peter Trimmel email="peter.trimmel@live.com"</author>
// <modified>Friday, 7th April 2023 11:17:41 author="Peter Trimmel"</modified>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

// Set the software version and the board info for the SystemInfo.
// Using the Visual Studio Code extension "Auto Version Increment",
// this include file is setup to be the primary file maintaining the version number
// in the extension settings. The SOFTWARE_VERSION string will be update automatically.
struct Auto
{
    static constexpr const char* const SOFTWARE_VERSION = "V1.3.0.55";
    static constexpr const char* const LAST_MODIFIED_DATE = __DATE__;
};
