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
 *
 ************************************************************************************************/

#include "main.h"

static DX_TIMER_HANDLER(publish_message_handler)
{
    static int msgId = 0;

    if (azure_connected)
    {
        // clang-format off
        // Serialize environment as JSON
        bool serialization_result = dx_jsonSerialize(msgBuffer, sizeof(msgBuffer), 4,
            DX_JSON_INT, "msgId", msgId++,
            DX_JSON_INT, "temperature", 25,
            DX_JSON_INT, "humidity", 70,
            DX_JSON_INT, "pressure", 1012);
        // clang-format on

        if (serialization_result)
        {
            dx_Log_Debug("%s\n", msgBuffer);
            dx_azurePublish(msgBuffer, strlen(msgBuffer), messageProperties, NELEMS(messageProperties), &contentProperties);
        }
        else
        {
            dx_terminate(APP_ExitCode_Telemetry_Buffer_Too_Small);
        }
    }
}
DX_TIMER_HANDLER_END

// Set Network Connected state LED
static void ConnectionStatus(bool connection_state)
{
    azure_connected = connection_state;
    dx_Log_Debug("Azure connection state: %s\n", connection_state ? "Connected" : "Disconnected");
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timers.
/// </summary>
static void InitPeripheralsAndHandlers(void)
{
    dx_Log_Debug_Init(debug_msg_buffer, sizeof(debug_msg_buffer));
    dx_azureConnect(&dx_config, dx_config.network_interface, IOT_PLUG_AND_PLAY_MODEL_ID);
    dx_azureRegisterConnectionChangedNotification(ConnectionStatus);
    dx_timerSetStart(timer_binding_set, NELEMS(timer_binding_set));
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    dx_timerSetStop(timer_binding_set, NELEMS(timer_binding_set));
    dx_timerEventLoopStop();
}

int main(int argc, char *argv[])
{
    dx_registerTerminationHandler();
    if (!dx_configParseCmdLineArguments(argc, argv, &dx_config))
    {
        return dx_getTerminationExitCode();
    }

    InitPeripheralsAndHandlers();

    // This call blocks until termination requested
    dx_eventLoopRun();

    ClosePeripheralsAndHandlers();
    return dx_getTerminationExitCode();
}