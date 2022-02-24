#pragma once

#include "hw/azure_sphere_remotex.h"

#include "app_exit_codes.h"
#include "dx_azure_iot.h"
#include "dx_config.h"
#include "dx_device_twins.h"
#include "dx_direct_methods.h"
#include "dx_gpio.h"
#include "dx_i2c.h"
#include "dx_json_serializer.h"
#include "dx_pwm.h"
#include "dx_timer.h"
#include "dx_utilities.h"
#include "dx_version.h"
#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"

#include "74HC165.h"
#include "applibs/remotex.h"
#include "applibs/storage.h"
#include "as1115.h"
#include "eeprom2.h"
#include "graphics.h"
#include "hm330x.h"
#include "light_sensor.h"
#include "max7219.h"

// https://docs.microsoft.com/en-us/azure/iot-pnp/overview-iot-plug-and-play
#define IOT_PLUG_AND_PLAY_MODEL_ID "dtmi:com:example:azuresphere:labmonitor;1"
#define NETWORK_INTERFACE "wlan0"
#define SAMPLE_VERSION_NUMBER "1.0"

/***********************************************************************************************************
 * Forward declarations
 **********************************************************************************************************/

static DX_DECLARE_DEVICE_TWIN_HANDLER(dt_panel_message_handler);
static DX_DECLARE_DEVICE_TWIN_HANDLER(dt_set_target_temperature_handler);
static DX_DECLARE_DIRECT_METHOD_HANDLER(gpio_handler);
static DX_DECLARE_TIMER_HANDLER(ButtonPressCheckHandler);
static DX_DECLARE_TIMER_HANDLER(publish_buzzer_off_handler);
static DX_DECLARE_TIMER_HANDLER(publish_led_off_handler);
static DX_DECLARE_TIMER_HANDLER(read_telemetry_handler);
static DX_DECLARE_TIMER_HANDLER(tmr_read_light_level_handler);
static DX_DECLARE_TIMER_HANDLER(tmr_read_panel_buttons_handler);
static DX_DECLARE_TIMER_HANDLER(tmr_read_pm_sensor_handler);
static DX_DECLARE_TIMER_HANDLER(tmr_update_display_handler);
static DX_DECLARE_TIMER_HANDLER(update_device_twins_handler);
static void alert_temperature_handler(uv_timer_t *handle);
static void publish_message_handler(uv_timer_t *handle);

/***********************************************************************************************************
 * Declare global variables
 **********************************************************************************************************/

DX_USER_CONFIG dx_config;

#define Log_Debug(f_, ...) dx_Log_Debug((f_), ##__VA_ARGS__)
static char Log_Debug_Time_buffer[256];

#define JSON_MESSAGE_BYTES 256
static char msgBuffer[JSON_MESSAGE_BYTES] = {0};

static bool azure_connected;
static int target_temperature;

static int msgId = 0;
static int eeprom_write_len = 0;

static const int logger_block_size = 128;
static int logger_fd = -1;
static int block_id = -1;

ENVIRONMENT telemetry;

hm330x_t pm_sensor;

max7219_t panel8x8_a = {.interfaceId = SPI_MASTER, .chipSelectId = MT3620_SPI_CS_A, .busSpeed = 10000000, .bitmap = {0}};
max7219_t panel8x8_b = {.interfaceId = SPI_MASTER, .chipSelectId = MT3620_SPI_CS_B, .busSpeed = 10000000, .bitmap = {0}};

as1115_t retro_click = {.interfaceId = I2C_MASTER, .bitmap64 = 0, .keymap = 0, .debouncePeriodMilliseconds = 500};
eeprom2_t eeprom2_logger = {.interfaceId = SPI_MASTER, .chipSelectId = MT3620_SPI_CS_A};
key4x4_t key4x4 = {.interfaceId = SPI_MASTER, .chipSelectId = MT3620_SPI_CS_B, .busSpeed = 10000000, .bitmap = 0, .debouncePeriodMilliseconds = 500};

/***********************************************************************************************************
 * Common content properties for publish messages to IoT Hub/Central
 **********************************************************************************************************/

static DX_MESSAGE_PROPERTY *messageProperties[] = {&(DX_MESSAGE_PROPERTY){.key = "appid", .value = "hvac"}, &(DX_MESSAGE_PROPERTY){.key = "type", .value = "telemetry"},
                                                   &(DX_MESSAGE_PROPERTY){.key = "schema", .value = "1"}};

static DX_MESSAGE_CONTENT_PROPERTIES contentProperties = {.contentEncoding = "utf-8", .contentType = "application/json"};

/***********************************************************************************************************
 * declare gpio bindings
 **********************************************************************************************************/

static DX_GPIO_BINDING gpio_button_a = {.pin = BUTTON_A, .direction = DX_INPUT, .name = "BUTTON_A"};
static DX_GPIO_BINDING gpio_led = {.pin = USER_LED_RED, .direction = DX_OUTPUT, .initialState = false, .invertPin = true, .name = "Alert LED"};
static DX_GPIO_BINDING gpio_publish = {.pin = STATUS_LED, .direction = DX_OUTPUT, .initialState = false, .invertPin = true, .name = "APP LED"};
static DX_GPIO_BINDING gpio_wlan = {.pin = WLAN_STATUS_LED_YELLOW, .direction = DX_OUTPUT, .initialState = false, .invertPin = true, .name = "WLAN LED"};

/***********************************************************************************************************
 * declare timer bindings
 **********************************************************************************************************/

static DX_TIMER_BINDING tmr_buttonPressCheckTimer = {.repeat = &(struct timespec){0, 100 * ONE_MS}, .name = "tmr_buttonPressCheckTimer", .handler = ButtonPressCheckHandler};
static DX_TIMER_BINDING tmr_publish_buzzer_off_oneshot = {.name = "tmr_publish_buzzer_off_oneshot", .handler = publish_buzzer_off_handler};
static DX_TIMER_BINDING tmr_publish_led_off = {.name = "tmr_publish_led_off", .handler = publish_led_off_handler};
static DX_TIMER_BINDING tmr_publish_message = {.repeat = &(struct timespec){4, 0}, .name = "tmr_publish_message", .handler = publish_message_handler};
static DX_TIMER_BINDING tmr_read_environment = {.repeat = &(struct timespec){30, 0}, .name = "tmr_read_environment", .handler = read_telemetry_handler};
static DX_TIMER_BINDING tmr_read_light_level = {.repeat = &(struct timespec){2, 0}, .name = "tmr_read_light_level", .handler = tmr_read_light_level_handler};
static DX_TIMER_BINDING tmr_read_panel_buttons = {.repeat = &(struct timespec){0, 250 * ONE_MS}, .name = "tmr_read_panel_buttons", .handler = tmr_read_panel_buttons_handler};
static DX_TIMER_BINDING tmr_read_pm_sensor = {.repeat = &(struct timespec){15, 0}, .name = "tmr_read_pm_sensor", .handler = tmr_read_pm_sensor_handler};
static DX_TIMER_BINDING tmr_temperature_alert = {.repeat = &(struct timespec){1, 0}, .name = "tmr_temperature_alert", .handler = alert_temperature_handler};
static DX_TIMER_BINDING tmr_update_device_twins = {.repeat = &(struct timespec){10, 0}, .name = "tmr_update_device_twins", .handler = update_device_twins_handler};
static DX_TIMER_BINDING tmr_update_display = {.repeat = &(struct timespec){0, 500 * ONE_MS}, .name = "tmr_update_display", .handler = tmr_update_display_handler};

/***********************************************************************************************************
 * declare device twin bindings
 **********************************************************************************************************/

static DX_DEVICE_TWIN_BINDING dt_humidity = {.propertyName = "humidity", .twinType = DX_DEVICE_TWIN_INT};
static DX_DEVICE_TWIN_BINDING dt_panel_msg = {.propertyName = "PanelMessage", .twinType = DX_DEVICE_TWIN_STRING, .handler = dt_panel_message_handler};
static DX_DEVICE_TWIN_BINDING dt_pressure = {.propertyName = "pressure", .twinType = DX_DEVICE_TWIN_INT};
static DX_DEVICE_TWIN_BINDING dt_startup_utc = {.propertyName = "StartupUtc", .twinType = DX_DEVICE_TWIN_STRING};
static DX_DEVICE_TWIN_BINDING dt_sw_version = {.propertyName = "SoftwareVersion", .twinType = DX_DEVICE_TWIN_STRING};
static DX_DEVICE_TWIN_BINDING dt_target_temperature = {.propertyName = "TargetTemperature", .twinType = DX_DEVICE_TWIN_INT, .handler = dt_set_target_temperature_handler};
static DX_DEVICE_TWIN_BINDING dt_temperature = {.propertyName = "temperature", .twinType = DX_DEVICE_TWIN_INT};

/***********************************************************************************************************
 * declare direct methods
 **********************************************************************************************************/

static DX_DIRECT_METHOD_BINDING dm_device_led_off = {.methodName = "LedOff", .handler = gpio_handler, .context = "LED Off"};
static DX_DIRECT_METHOD_BINDING dm_device_led_on = {.methodName = "LedOn", .handler = gpio_handler, .context = "LED On"};

/***********************************************************************************************************
 * declare pwm bindings
 **********************************************************************************************************/

static DX_PWM_CONTROLLER pwm_click_controller = {.controllerId = 0, .name = "PWM Click Controller"};
static DX_PWM_BINDING pwm_buzz_click = {.pwmController = &pwm_click_controller, .channelId = 0, .name = "click 1 buzz"};

/***********************************************************************************************************
 * declare i2c bindings
 **********************************************************************************************************/

DX_I2C_BINDING i2c_as1115_retro = {.interfaceId = I2C_MASTER, .speedInHz = I2C_BUS_SPEED_FAST_PLUS, .name = "i2c_as1115_retro"};
DX_I2C_BINDING i2c_hm330x_pm_sensor = {.interfaceId = I2C_MASTER, .speedInHz = I2C_BUS_SPEED_FAST_PLUS, .name = "hm330x i2c_hm330x_pm_sensor"};
DX_I2C_BINDING i2c_onboard_sensors = {.interfaceId = I2C_MASTER, .speedInHz = I2C_BUS_SPEED_FAST_PLUS, .name = "i2c_onboard_sensors"};

/***********************************************************************************************************
 * declare binding sets
 **********************************************************************************************************/

static DX_I2C_BINDING *i2c_bindings[] = {&i2c_onboard_sensors, &i2c_as1115_retro, &i2c_hm330x_pm_sensor};
static DX_PWM_BINDING *pwm_bindings[] = {&pwm_buzz_click};
static DX_DEVICE_TWIN_BINDING *device_twin_bindings[] = {&dt_humidity, &dt_pressure, &dt_temperature, &dt_sw_version, &dt_startup_utc, &dt_target_temperature, &dt_panel_msg};
static DX_DIRECT_METHOD_BINDING *direct_method_bindings[] = {&dm_device_led_on, &dm_device_led_off};
static DX_GPIO_BINDING *gpio_bindings[] = {&gpio_led, &gpio_wlan, &gpio_publish, &gpio_button_a};
static DX_TIMER_BINDING *timer_bindings[] = {
    &tmr_buttonPressCheckTimer,
    &tmr_publish_buzzer_off_oneshot,
    &tmr_publish_led_off,
    &tmr_read_environment,
    &tmr_read_light_level,
    &tmr_read_panel_buttons,
    &tmr_read_pm_sensor,
    &tmr_temperature_alert,
    &tmr_update_device_twins,
    &tmr_update_display};