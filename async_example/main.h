#pragma once

#include "app_exit_codes.h"
#include "dx_async.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_utilities.h"
#include "dx_version.h"
#include <applibs/log.h>

// Forward declarations
static DX_DECLARE_ASYNC_HANDLER(async_test_handler);
static DX_DECLARE_ASYNC_HANDLER(async_test2_handler);
static DX_DECLARE_TIMER_HANDLER(test1_handler);
static DX_DECLARE_TIMER_HANDLER(test2_handler);

int count = 0;
static char debug_msg_buffer[128] = {};

static DX_TIMER_BINDING tmr_test1 = {.delay = &(struct timespec){0, 1 * ONE_MS}, .name = "tmr_test1", .handler = test1_handler};
static DX_TIMER_BINDING tmr_test2 = {.delay = &(struct timespec){0, 1 * ONE_MS}, .name = "tmr_test2", .handler = test2_handler};

static DX_ASYNC_BINDING async_test1 = {.name = "async_test", .handler = async_test_handler};
static DX_ASYNC_BINDING async_test2 = {.name = "async_test2", .handler = async_test2_handler};

DX_TIMER_BINDING *timer_bindings[] = {&tmr_test1, &tmr_test2};
DX_ASYNC_BINDING *asyncSet[] = {&async_test1, &async_test2};