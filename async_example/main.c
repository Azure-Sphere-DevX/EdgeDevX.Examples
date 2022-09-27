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

/// <summary>
///  Timer handler 1
/// </summary>
DX_TIMER_HANDLER(test1_handler)
{
    if (count % 1000 == 0)
    {
        Log_Debug("Data1:%d\n", count);
    }
}
DX_TIMER_HANDLER_END

/// <summary>
///  Timer handler 2
/// </summary>
DX_TIMER_HANDLER(test2_handler)
{
    if (count % 1000 == 0)
    {
        Log_Debug("Data2:%d\n", count);
    }
}
DX_TIMER_HANDLER_END

/// <summary>
///  Async handler to set oneshot timer.
/// </summary>
DX_ASYNC_HANDLER(async_test2_handler, handle)
{
    // count = *(int *)handle->data;
    dx_timerOneShotSet(&tmr_test1, &(struct timespec){0, 1});
}
DX_ASYNC_HANDLER_END

/// <summary>
///  Async handler to set oneshot timer.
/// </summary>
DX_ASYNC_HANDLER(async_test_handler, handle)
{
    // count = *(int *)handle->data;
    dx_timerOneShotSet(&tmr_test2, &(struct timespec){0, 1});
}
DX_ASYNC_HANDLER_END

/// <summary>
///  Demo thread to show how to use async handler.
/// </summary>
static void *count_thread(void *arg)
{
    while (true)
    {
        count++;

        if (count % 2 == 0)
        {
            dx_asyncSend(&async_test1, (void *)&count);
            // dx_timerOneShotSet(&tmr_test1, &(struct timespec){0, 1});
        }
        else
        {
            dx_asyncSend(&async_test2, (void *)&count);
            // dx_timerOneShotSet(&tmr_test2, &(struct timespec){0, 1});
        }
        nanosleep(&(struct timespec){0, 2}, NULL);
    }
    return NULL;
}

/// <summary>
///  Initialize peripherals, device twins, direct methods, timer_bindings.
/// </summary>
static void InitPeripheralsAndHandlers(void)
{
    dx_Log_Debug_Init(debug_msg_buffer, sizeof(debug_msg_buffer));
    dx_asyncSetInit(asyncSet, NELEMS(asyncSet));
    dx_timerSetStart(timer_bindings, NELEMS(timer_bindings));

    dx_startThreadDetached(count_thread, NULL, "count_thread");
}

/// <summary>
///     Close peripherals and handlers.
/// </summary>
static void ClosePeripheralsAndHandlers(void)
{
    dx_timerSetStop(timer_bindings, NELEMS(timer_bindings));
    dx_timerEventLoopStop();
}

int main(int argc, char *argv[])
{
    dx_registerTerminationHandler();

    InitPeripheralsAndHandlers();

    // This call blocks until termination requested
    dx_eventLoopRun();

    ClosePeripheralsAndHandlers();

    return dx_getTerminationExitCode();
}