#pragma once

#include "dx_azure_iot.h"
#include "dx_config.h"
#include "dx_direct_methods.h"
#include "app_exit_codes.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_utilities.h"

// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"

// Forward declarations
static DX_DECLARE_DIRECT_METHOD_HANDLER(LightControlHandler);
static DX_DECLARE_TIMER_HANDLER(DelayRestartDeviceTimerHandler);
static DX_DECLARE_TIMER_HANDLER(LedOffToggleHandler);

// Variables
DX_USER_CONFIG dx_config;
static char debug_msg_buffer[128] = {0};

/****************************************************************************************
 * Timer Bindings
 ****************************************************************************************/
static DX_TIMER_BINDING led_off_oneshot_timer = {.repeat = &(struct timespec){0, 0}, .name = "led_off_oneshot_timer", .handler = LedOffToggleHandler};

/****************************************************************************************
 * Azure IoT Direct Method Bindings
 ****************************************************************************************/
static DX_DIRECT_METHOD_BINDING dm_light_control = {.methodName = "LightControl", .handler = LightControlHandler};

// All direct methods referenced in direct_method_bindings will be subscribed to in
// the InitPeripheralsAndHandlers function
DX_DIRECT_METHOD_BINDING *direct_method_bindings[] = {&dm_light_control};

// All timers referenced in timers with be opened in the InitPeripheralsAndHandlers function
DX_TIMER_BINDING *timers[] = {&led_off_oneshot_timer};