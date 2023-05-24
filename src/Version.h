// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Version.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>9-4-2023 7:49 PM</created>
// <modified>10-4-2023 10:56 AM</modified>
// <author>Peter Trimmel</author>
// <summary>                                                                                                           
//   The convention for versioning is quite simple:
//
//     {major}.{minor}.{patch}-{tag}
//     
//     {major} - is only incremented if the release has breaking changes (includes bug fixes which have breaking behavioural changes)
//     {minor} - is incremented if the release has new non-breaking features
//     {patch} - is incremented if the release only contains non-breaking bug fixes
//     {tag}   - is optional and denotes a pre-release of the version
//
//   Only one number should be incremented per release, and all lower parts should be reset to 0.
//   The GitVersion configuration file is used to set continuous deployment, the version number, and the fallback tag:
//     
//     next-version: 1.5.1
//     mode: ContinuousDeployment
//     update-build-number: true
//     continuous-delivery-fallback-tag: beta
//     
// </summary>     
// --------------------------------------------------------------------------------------------------------------------
#pragma once

struct Auto
{
    static constexpr const char* const SOFTWARE_VERSION = "V1.0.12-beta.27 2023-05-24";
    static constexpr const char* const LAST_MODIFIED_DATE = __DATE__;
    static constexpr const char* const LAST_MODIFIED_TIME = __TIME__;
};

