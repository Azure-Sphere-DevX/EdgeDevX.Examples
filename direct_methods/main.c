/* Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * This example is built on the Azure Sphere DevX library.
 *   1. DevX is an Open Source community-maintained implementation of the Azure Sphere SDK samples.
 *   2. DevX is a modular library that simplifies common development scenarios.
 *        - You can focus on your solution, not the plumbing.
 *   3. DevX documentation is maintained at https://github.com/gloveboxes/AzureSphereDevX/wiki
 *	 4. The DevX library is not a substitute for understanding the Azure Sphere SDK Samples.
 *          - https://github.com/Azure/azure-sphere-samples
 *
 * DEVELOPER BOARD SELECTION
 *
 * The following developer boards are supported.
 *
 *	 1. AVNET Azure Sphere Starter Kit.
 *   2. AVNET Azure Sphere Starter Kit Revision 2.
 *	 3. Seeed Studio Azure Sphere MT3620 Development Kit aka Reference Design Board or rdb.
 *	 4. Seeed Studio Seeed Studio MT3620 Mini Dev Board.
 *
 * ENABLE YOUR DEVELOPER BOARD
 *
 * Each Azure Sphere developer board manufacturer maps pins differently. You need to select the
 *    configuration that matches your board.
 *
 * Follow these steps:
 *
 *	 1. Open CMakeLists.txt.
 *	 2. Uncomment the set command that matches your developer board.
 *	 3. Click File, then Save to auto-generate the CMake Cache.
 *
 ************************************************************************************************/

#include "main.h"

/// <summary>
/// One shot timer handler to turn off Alert LED
/// </summary>
static DX_TIMER_HANDLER(LedOffToggleHandler)
{
    dx_Log_Debug("LED turned off\n");
}
DX_TIMER_HANDLER_END

// Direct method name = LightControl, json payload = {"State": true, "Duration":2} or {"State":
// false, "Duration":2}
static DX_DIRECT_METHOD_HANDLER(LightControlHandler, json, directMethodBinding, responseMsg)
{
    char duration_str[] = "Duration";
    char state_str[] = "State";
    bool requested_state;
    int requested_duration_seconds;

    JSON_Object *jsonObject = json_value_get_object(json);
    if (jsonObject == NULL) {
        return DX_METHOD_FAILED;
    }

    // check JSON properties sent through are the correct type
    if (!json_object_has_value_of_type(jsonObject, duration_str, JSONNumber) ||
        !json_object_has_value_of_type(jsonObject, state_str, JSONBoolean)) {
        return DX_METHOD_FAILED;
    }

    requested_state = (bool)json_object_get_boolean(jsonObject, state_str);
    dx_Log_Debug("State %d \n", requested_state);

    requested_duration_seconds = (int)json_object_get_number(jsonObject, duration_str);
    dx_Log_Debug("Duration %d \n", requested_duration_seconds);

    if (!IN_RANGE(requested_duration_seconds, 1, 120)) {
        return DX_METHOD_FAILED;
    }

    dx_Log_Debug("Requested Light state: %s\n", requested_state ? "On" : "Off");

    // If the request was to turn on then turn off after duration seconds
    if (requested_state) {
        dx_timerOneShotSet(&led_off_oneshot_timer, &(struct timespec){requested_duration_seconds, 0});
    }

    return DX_METHOD_SUCCEEDED;
}
DX_DIRECT_METHOD_HANDLER_END

static void ConnectionStatus(bool connection_state)
{
    dx_Log_Debug("Azure connection state: %s\n", connection_state ? "Connected": "Disconnected");
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralsAndHandlers(void)
{
    dx_Log_Debug_Init(debug_msg_buffer, sizeof(debug_msg_buffer));
    dx_azureConnect(&dx_config, dx_config.network_interface, IOT_PLUG_AND_PLAY_MODEL_ID);
    dx_timerSetStart(timers, NELEMS(timers));
    dx_directMethodSubscribe(direct_method_bindings, NELEMS(direct_method_bindings));

    dx_azureRegisterConnectionChangedNotification(ConnectionStatus);
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    dx_timerSetStop(timers, NELEMS(timers));
    dx_directMethodUnsubscribe();
    dx_timerEventLoopStop();
}

int main(int argc, char *argv[])
{
    dx_registerTerminationHandler();
    if (!dx_configParseCmdLineArguments(argc, argv, &dx_config)) {
        return dx_getTerminationExitCode();
    }
    InitPeripheralsAndHandlers();

    // This call blocks until termination requested
    dx_eventLoopRun();

    ClosePeripheralsAndHandlers();
    return dx_getTerminationExitCode();
}