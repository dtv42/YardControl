// --------------------------------------------------------------------------------------------------------------------
// <copyright file="GpioInputs.cpp" company="DTV-Online">
//   Copyright (c) 2023 Dr. Peter Trimmel. All rights reserved.
// </copyright>
// <license>
//   Licensed under the MIT license. See the LICENSE file in the project root for more information.
// </license>
// <created>17-4-2023 7:35 AM</created>
// <modified>17-4-2023 8:43 AM</modified>
// <author>Peter Trimmel</author>
// --------------------------------------------------------------------------------------------------------------------
// Disable all traces when set to 0.
#define ARDUINOTRACE_ENABLE 1

#include <ArduinoTrace.h>
#include "AppSettings.h"
#include "Actuator.h"
#include "GpioInputs.h"

extern AppSettings Settings;
extern LinearActuator Actuator;

#pragma region Input Callbacks

/// <summary>
/// Callback triggered when the stepper alarm output goes HIGH.
/// </summary>
/// <param name="pin">The input pin number.</param>
void alarmOnCallback(uint8_t pin)
{
    TRACE(); DUMP(pin);
    Actuator.alarmOn(pin);
}

/// <summary>
/// Callback triggered when the stepper alarm output goes LOW.
/// </summary>
/// <param name="pin">The input pin number.</param>
void alarmOffCallback(uint8_t pin)
{
    TRACE(); DUMP(pin);
    Actuator.alarmOff(pin);
}

/// <summary>
/// Callback triggered when the (debounced) switch is closed.
/// </summary>
/// <param name="pin">The input pin number.</param>
void switchOnCallback(uint8_t pin)
{
    TRACE(); DUMP(pin);
    Actuator.switchOn(pin);
}

/// <summary>
/// Callback triggered when the (debounced) switch is opened.
/// </summary>
/// <param name="pin">The input pin number.</param>
void switchOffCallback(uint8_t pin)
{
    TRACE(); DUMP(pin);
    Actuator.switchOff(pin);
}

#pragma endregion

void GpioInputs::init()
{
    TRACE();

    StepperAlarm.setup(Settings.Stepper.PinALM);
    SwitchStop.setup(Settings.Actuator.SwitchStop);
    SwitchLimit1.setup(Settings.Actuator.SwitchLimit1);
    SwitchLimit2.setup(Settings.Actuator.SwitchLimit2);

    StepperAlarm.registerCallbacks(alarmOnCallback, alarmOffCallback, NULL, NULL);
    SwitchStop.registerCallbacks(switchOnCallback, switchOffCallback, NULL, NULL);
    SwitchLimit1.registerCallbacks(switchOnCallback, switchOffCallback, NULL, NULL);
    SwitchLimit2.registerCallbacks(switchOnCallback, switchOffCallback, NULL, NULL);
}

void GpioInputs::run()
{
    unsigned long now = millis();

    StepperAlarm.process(now);
    SwitchStop.process(now);
    SwitchLimit1.process(now);
    SwitchLimit2.process(now);
}
