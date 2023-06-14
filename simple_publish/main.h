#pragma once

#include "app_exit_codes.h"
#include "dx_azure_iot.h"
#include "dx_config.h"
#include "dx_json_serializer.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_utilities.h"

// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"

static DX_MESSAGE_PROPERTY *messageProperties[] = {&(DX_MESSAGE_PROPERTY){.key = "appid", .value = "hvac"}, &(DX_MESSAGE_PROPERTY){.key = "type", .value = "telemetry"},
                                                   &(DX_MESSAGE_PROPERTY){.key = "schema", .value = "1"}};

static DX_MESSAGE_CONTENT_PROPERTIES contentProperties = {.contentEncoding = "utf-8",
                                                          .contentType = "application/json"};

// Number of bytes to allocate for the JSON telemetry message for IoT Central
#define JSON_MESSAGE_BYTES 256
static char msgBuffer[JSON_MESSAGE_BYTES] = {0};
static char debug_msg_buffer[128] = {0};

DX_USER_CONFIG dx_config;
static bool azure_connected = false;

// Forward declarations
static DX_DECLARE_TIMER_HANDLER(publish_message_handler);

/****************************************************************************************
 * Bindings
 ****************************************************************************************/
static DX_TIMER_BINDING tmr_publish_message = {.repeat = &(struct timespec){4, 0}, .name = "tmr_publish_message", .handler = publish_message_handler};

// All bindings referenced in the bindings sets will be initialised in the
// InitPeripheralsAndHandlers function
DX_TIMER_BINDING *timer_binding_set[] = {&tmr_publish_message};
