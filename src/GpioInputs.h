// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GpioInputs.h" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>17-4-2023 7:33 AM</created>
// <modified>17-4-2023 8:12 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
#pragma once

#include <InputDebounce.h>

class GpioInputs
{
 public:
     InputDebounce StepperAlarm;                 // Stepper alarm input .
     InputDebounce SwitchStop;                   // Emergency stop switch input.
     InputDebounce SwitchLimit1;                 // Limit switch 1 (calibration).
     InputDebounce SwitchLimit2;                 // Limit switch 2 (end of actuator).
     
     void init();
     void run();
};

