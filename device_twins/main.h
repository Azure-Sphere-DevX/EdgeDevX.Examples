#pragma once

#include "app_exit_codes.h"
#include "dx_azure_iot.h"
#include "dx_config.h"
#include "dx_device_twins.h"
#include "dx_device_twins.h"
#include "dx_json_serializer.h"
#include "dx_terminate.h"
#include "dx_timer.h"
#include "dx_utilities.h"

#include <ctype.h>
#include <stdlib.h>
#include <time.h>

// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"

// Number of bytes to allocate for the JSON telemetry message for IoT Central
#define JSON_MESSAGE_BYTES 256
static char msgBuffer[JSON_MESSAGE_BYTES] = {0};
static char debug_msg_buffer[128] = {0};

typedef struct
{
    int temperature;
    int humidity;
    int pressure;
} SENSOR_T;

typedef struct
{
    SENSOR_T latest;
    SENSOR_T previous;
    bool validated;
} ENVIRONMENT_T;

ENVIRONMENT_T environment;

#define LOCAL_DEVICE_TWIN_PROPERTY 64
static char copy_of_property_value[LOCAL_DEVICE_TWIN_PROPERTY];

DX_USER_CONFIG dx_config;
static bool azure_connected = false;

// Forward declarations
static DX_DECLARE_DEVICE_TWIN_HANDLER(dt_copy_string_handler);
static DX_DECLARE_DEVICE_TWIN_HANDLER(dt_desired_sample_rate_handler);
static DX_DECLARE_DEVICE_TWIN_HANDLER(dt_gpio_handler);
static DX_DECLARE_TIMER_HANDLER(read_sensor_handler);
static DX_DECLARE_TIMER_HANDLER(report_now_handler);

static DX_MESSAGE_PROPERTY *sensorErrorProperties[] = {
    &(DX_MESSAGE_PROPERTY){.key = "appid", .value = "hvac"},
    &(DX_MESSAGE_PROPERTY){.key = "type", .value = "SensorError"},
    &(DX_MESSAGE_PROPERTY){.key = "schema", .value = "1"}};

static DX_MESSAGE_CONTENT_PROPERTIES contentProperties = {.contentEncoding = "utf-8",
                                                          .contentType = "application/json"};

/****************************************************************************************
 * Bindings
 ****************************************************************************************/
static DX_TIMER_BINDING report_now_timer = {.repeat = &(struct timespec){6, 0}, .name = "report_now_timer", .handler = report_now_handler};
static DX_TIMER_BINDING tmr_read_sensor = {.repeat = &(struct timespec){2, 0}, .name = "tmr_report_properties", .handler = read_sensor_handler};


// All bindings referenced in the bindings sets will be initialised in the
// InitPeripheralsAndHandlers function
DX_TIMER_BINDING *timer_binding_set[] = {&report_now_timer, &tmr_read_sensor};

/****************************************************************************************
 * Azure IoT Device Twin Bindings
 ****************************************************************************************/
static DX_DEVICE_TWIN_BINDING dt_desired_sample_rate = {.propertyName = "DesiredSampleRate",
                                                        .twinType = DX_DEVICE_TWIN_INT,
                                                        .handler = dt_desired_sample_rate_handler};

static DX_DEVICE_TWIN_BINDING dt_sample_string = {.propertyName = "SampleString",
                                                  .twinType = DX_DEVICE_TWIN_STRING,
                                                  .handler = dt_copy_string_handler};

static DX_DEVICE_TWIN_BINDING dt_reported_temperature = {.propertyName = "ReportedTemperature",
                                                         .twinType = DX_DEVICE_TWIN_INT};

static DX_DEVICE_TWIN_BINDING dt_reported_humidity = {.propertyName = "ReportedHumidity",
                                                      .twinType = DX_DEVICE_TWIN_INT};

static DX_DEVICE_TWIN_BINDING dt_reported_utc = {.propertyName = "ReportedUTC",
                                                 .twinType = DX_DEVICE_TWIN_STRING};

static DX_DEVICE_TWIN_BINDING dt_user_led_red = {.propertyName = "userLedRed",
                                                 .twinType = DX_DEVICE_TWIN_BOOL,
                                                 .handler = dt_gpio_handler,
                                                 .context = "Red LED example context object"};

static DX_DEVICE_TWIN_BINDING dt_user_led_green = {.propertyName = "userLedGreen",
                                                   .twinType = DX_DEVICE_TWIN_BOOL,
                                                   .handler = dt_gpio_handler,
                                                   .context = "Green LED example context object"};

static DX_DEVICE_TWIN_BINDING dt_user_led_blue = {.propertyName = "userLedBlue",
                                                  .twinType = DX_DEVICE_TWIN_BOOL,
                                                  .handler = dt_gpio_handler,
                                                  .context = "Blue LED example context object"};

// All device twins listed in device_twin_bindings will be subscribed to in
// the InitPeripheralsAndHandlers function
DX_DEVICE_TWIN_BINDING *device_twin_bindings[] = {
    &dt_desired_sample_rate, &dt_reported_temperature, &dt_reported_humidity, &dt_reported_utc,
    &dt_sample_string,       &dt_user_led_red,         &dt_user_led_green,    &dt_user_led_blue};