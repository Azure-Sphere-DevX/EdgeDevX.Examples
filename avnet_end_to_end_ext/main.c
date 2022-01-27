#include "main.h"

/***********************************************************************************************************
 * Core functions
 *
 * Read environment sensor
 * Publish data to Azure IoT Hub/Central
 * Update device twins
 **********************************************************************************************************/

/// <summary>
/// Determine if telemetry value changed. If so, update it's device twin
/// </summary>
/// <param name="new_value"></param>
/// <param name="previous_value"></param>
/// <param name="device_twin"></param>
static void device_twin_update(int *latest_value, int *previous_value, DX_DEVICE_TWIN_BINDING *device_twin)
{
    if (*latest_value != *previous_value)
    {
        *previous_value = *latest_value;
        dx_deviceTwinReportValue(device_twin, latest_value);
    }
}

/// <summary>
/// Only update device twins if data changed to minimize network and cloud costs
/// </summary>
/// <param name="temperature"></param>
/// <param name="pressure"></param>
static DX_TIMER_HANDLER(update_device_twins_handler)
{
    if (telemetry.valid && azure_connected)
    {
        device_twin_update(&telemetry.latest.temperature, &telemetry.previous.temperature, &dt_temperature);
        device_twin_update(&telemetry.latest.pressure, &telemetry.previous.pressure, &dt_pressure);
        device_twin_update(&telemetry.latest.humidity, &telemetry.previous.humidity, &dt_humidity);
    }
}
DX_TIMER_HANDLER_END

static DX_TIMER_HANDLER(publish_led_off_handler)
{
    dx_gpioOff(&gpio_publish);
}
DX_TIMER_HANDLER_END

/// <summary>
/// Oneshot handler to turn off BUZZ Click buzzer
/// </summary>
/// <param name="eventLoopTimer"></param>
static DX_TIMER_HANDLER(publish_buzzer_off_handler)
{
    dx_pwmStop(&pwm_buzz_click);

    memset(msgBuffer, 0x00, sizeof(msgBuffer));
    if (block_id > 0 && eeprom2_read_bytes(&eeprom2_logger, block_id * logger_block_size, (uint8_t *)msgBuffer, eeprom_write_len) == eeprom_write_len)
    {
        dx_Log_Debug("EEPROM Block id:%.4d, %s\n", block_id, msgBuffer);
    }
}
DX_TIMER_HANDLER_END

static DX_TIMER_HANDLER(publish_message_handler)
{
    if (telemetry.valid && azure_connected)
    {
        // Serialize telemetry as JSON
        bool serialization_result = dx_jsonSerialize(msgBuffer, sizeof(msgBuffer), 8,
                                                     DX_JSON_INT, "MsgId", msgId,
                                                     DX_JSON_INT, "Temperature", telemetry.latest.temperature,
                                                     DX_JSON_INT, "Humidity", telemetry.latest.humidity,
                                                     DX_JSON_INT, "Pressure", telemetry.latest.pressure,
                                                     DX_JSON_INT, "Light", telemetry.latest.light,
                                                     DX_JSON_INT, "PM_1_0", pm_sensor.data.standard_particulate_matter_1_0,
                                                     DX_JSON_INT, "PM_2_5", pm_sensor.data.standard_particulate_matter_2_5,
                                                     DX_JSON_INT, "PM_3_0", pm_sensor.data.standard_particulate_matter_3_0);

        if (serialization_result)
        {
            // There are 520 128 byte blocks in 65k mutable storage
            block_id = msgId % 520;

            log_write((uint8_t*)msgBuffer, strlen(msgBuffer), block_id);
            memset(msgBuffer, 0x00, sizeof(msgBuffer));
            log_read((uint8_t*)msgBuffer, sizeof(msgBuffer), block_id);
            
            // There are 2048 128 blocks on 256K EEPROM2
            block_id = msgId % 2048;

            dx_Log_Debug("%s\n", msgBuffer);

            eeprom2_write_bytes(&eeprom2_logger, block_id * logger_block_size, (uint8_t *)msgBuffer, eeprom_write_len = strlen(msgBuffer));

            dx_pwmSetDutyCycle(&pwm_buzz_click, 5000, 1);
            dx_timerOneShotSet(&tmr_publish_buzzer_off_oneshot, &(struct timespec){2, 0 * ONE_MS});

            dx_azurePublish(msgBuffer, strlen(msgBuffer), messageProperties, NELEMS(messageProperties), &contentProperties);

            msgId++;
        }
        else
        {
            dx_Log_Debug("JSON Serialization failed: Buffer too small\n");
            dx_terminate(APP_ExitCode_Telemetry_Buffer_Too_Small);
        }
    }
}
DX_TIMER_HANDLER_END

/// <summary>
/// read_telemetry_handler callback handler called every 4 seconds
/// Environment sensors read and HVAC operating mode LED updated
/// </summary>
/// <param name="eventLoopTimer"></param>
static DX_TIMER_HANDLER(read_telemetry_handler)
{
    onboard_sensors_read(&telemetry.latest);

    // clang-format off
    telemetry.valid =
        IN_RANGE(telemetry.latest.temperature, -20, 50) &&
        IN_RANGE(telemetry.latest.pressure, 600, 1200) &&
        IN_RANGE(telemetry.latest.humidity, 0, 100);
    // clang-format on
}
DX_TIMER_HANDLER_END

/// <summary>
/// tmr_read_pm_sensor_handler callback handler called every 15 seconds
/// reads particulate matter sensor
/// </summary>
/// <param name="eventLoopTimer"></param>
static DX_TIMER_HANDLER(tmr_read_pm_sensor_handler)
{
    if (ConsumeEventLoopTimerEvent(eventLoopTimer) != 0)
    {
        dx_terminate(DX_ExitCode_ConsumeEventLoopTimeEvent);
        return;
    }

    hm330x_read(&pm_sensor);
}
DX_TIMER_HANDLER_END

static DX_TIMER_HANDLER(tmr_read_light_level_handler)
{
    telemetry.latest.light = avnet_get_light_level();
}
DX_TIMER_HANDLER_END

/// <summary>
/// Handler to check for Button Presses
/// </summary>
static DX_TIMER_HANDLER(ButtonPressCheckHandler)
{
    static GPIO_Value_Type buttonAState;

    if (dx_gpioStateGet(&gpio_button_a, &buttonAState))
    {
        dx_Log_Debug("Button pressed\n");
        dx_gpioOn(&gpio_publish);
        dx_timerOneShotSet(&tmr_publish_led_off, &(struct timespec){2, 0});
    }
}
DX_TIMER_HANDLER_END

static DX_TIMER_HANDLER(tmr_update_display_handler)
{
    static int charId;
    charId++;

    gfx_load_character((charId % 10) + 48, retro_click.bitmap);
    gfx_rotate_counterclockwise(retro_click.bitmap, 1, 1, retro_click.bitmap);
    gfx_reverse_panel(retro_click.bitmap);
    gfx_rotate_counterclockwise(retro_click.bitmap, 1, 1, retro_click.bitmap);
    as1115_panel_write(&retro_click);

    gfx_load_character((charId % 10) + 48, panel8x8_a.bitmap);
    gfx_reverse_panel(panel8x8_a.bitmap);
    gfx_rotate_counterclockwise(panel8x8_a.bitmap, 1, 1, panel8x8_a.bitmap);
    gfx_reverse_panel(panel8x8_a.bitmap);
    max7219_panel_write(&panel8x8_a);

    charId++;

    gfx_load_character((charId % 10) + 48, panel8x8_b.bitmap);
    gfx_reverse_panel(panel8x8_b.bitmap);
    gfx_rotate_counterclockwise(panel8x8_b.bitmap, 1, 1, panel8x8_b.bitmap);
    gfx_reverse_panel(panel8x8_b.bitmap);
    max7219_panel_write(&panel8x8_b);
}
DX_TIMER_HANDLER_END

static DX_TIMER_HANDLER(tmr_read_panel_buttons_handler)
{
    uint8_t button;

    if ((button = as1115_get_btn_position(&retro_click)) != 0)
    {
        dx_Log_Debug("Button %d pressed\n", button);
    }

    uint8_t button_pressed = c4x4key_get_btn_position(&key4x4);
    if (button_pressed != 0)
    {
        Log_Debug("74HC button pressed: %d\n", button_pressed);
    }
}
DX_TIMER_HANDLER_END

/***********************************************************************************************************
 * REMOTE OPERATIONS: DEVICE TWINS
 *
 * Set Temperature alert level
 **********************************************************************************************************/

/// <summary>
/// dt_set_target_temperature_handler callback handler is called when TargetTemperature device twin
/// message received HVAC operating mode LED updated and IoT Plug and Play device twin acknowledged
/// </summary>
/// <param name="deviceTwinBinding"></param>
static DX_DEVICE_TWIN_HANDLER(dt_set_target_temperature_handler, deviceTwinBinding)
{
    if (IN_RANGE(*(int *)deviceTwinBinding->propertyValue, 0, 50))
    {
        target_temperature = *(int *)dt_target_temperature.propertyValue;
        dx_Log_Debug("Device twin update: Target temperature alert: %d\n", target_temperature);
        dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_COMPLETED);
    }
    else
    {
        dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_ERROR);
    }
}
DX_DEVICE_TWIN_HANDLER_END

/// <summary>
/// dt_panel_message_handler callback handler is called when PanelMessage device twin message received.
/// </summary>
/// <param name="deviceTwinBinding"></param>
static DX_DEVICE_TWIN_HANDLER(dt_panel_message_handler, deviceTwinBinding)
{
    char *panel_message = (char *)deviceTwinBinding->propertyValue;

    if (dx_isStringPrintable(panel_message))
    {
        dx_Log_Debug("Device twin update: Panel Message: %s\n", panel_message);
        dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_COMPLETED);
    }
    else
    {
        dx_deviceTwinAckDesiredValue(deviceTwinBinding, deviceTwinBinding->propertyValue, DX_DEVICE_TWIN_RESPONSE_ERROR);
    }
}
DX_DEVICE_TWIN_HANDLER_END

static DX_TIMER_HANDLER(alert_temperature_handler)
{
    static bool previous_state, new_state;

    new_state = dt_target_temperature.propertyUpdated && telemetry.latest.temperature >= target_temperature;

    if (new_state != previous_state)
    {
        previous_state = new_state;
        dx_gpioStateSet(&gpio_led, new_state);
    }
}
DX_TIMER_HANDLER_END

/***********************************************************************************************************
 * REMOTE OPERATIONS: DIRECT METHIDS
 *
 * Turn LED light on/off
 **********************************************************************************************************/

// Direct method name for LedOn and LedOff
static DX_DIRECT_METHOD_HANDLER(gpio_handler, json, directMethodBinding, responseMsg)
{
    dx_Log_Debug("Direct method called: %s\n", (char *)directMethodBinding->context);

    return DX_METHOD_SUCCEEDED;
}
DX_DIRECT_METHOD_HANDLER_END

/************************************************************************************************************
 * Update software version and Azure connect UTC time device twins on first connection
 ***********************************************************************************************************/

/// <summary>
/// Called when the Azure connection status changes then unregisters this callback
/// </summary>
/// <param name="connected"></param>
static void startup_report(bool connected)
{
    snprintf(msgBuffer, sizeof(msgBuffer), "Firmware version: %s, DevX version: %s", SAMPLE_VERSION_NUMBER, AZURE_SPHERE_DEVX_VERSION);
    dx_deviceTwinReportValue(&dt_sw_version, msgBuffer);                                       // DX_TYPE_STRING
    dx_deviceTwinReportValue(&dt_startup_utc, dx_getCurrentUtc(msgBuffer, sizeof(msgBuffer))); // DX_TYPE_STRING

    dx_azureUnregisterConnectionChangedNotification(startup_report);
}

/***********************************************************************************************************
 * APPLICATION BASICS
 *
 * Set Azure connection state
 * Initialize CO2 sensor
 * Initialize resources
 * Close resources
 * Run the main event loop
 **********************************************************************************************************/

/// <summary>
/// Update local azure_connected with new connection status
/// </summary>
/// <param name="connected"></param>
void azure_connection_state(bool connected)
{
    azure_connected = connected;

    dx_gpioStateSet(&gpio_wlan, connected);
    dx_timerStateSet(&tmr_publish_message, connected);
}

static void InitPeripheralsAndHandlers(void)
{
    dx_Log_Debug_Init(Log_Debug_Time_buffer, sizeof(Log_Debug_Time_buffer));

    dx_Log_Debug("--------------------\n");
    dx_Log_Debug("%s\n", "Application starting");
    dx_Log_Debug("--------------------\n\n");

    dx_azureConnect(&dx_config, NETWORK_INTERFACE, IOT_PLUG_AND_PLAY_MODEL_ID);
    dx_gpioSetOpen(gpio_bindings, NELEMS(gpio_bindings));
    dx_pwmSetOpen(pwm_bindings, NELEMS(pwm_bindings));
    dx_i2cSetOpen(i2c_bindings, NELEMS(i2c_bindings));
    dx_timerSetStart(timer_bindings, NELEMS(timer_bindings));

    log_open(128);

    hm330x_init(i2c_hm330x_pm_sensor.fd, &pm_sensor);
    hm330x_read(&pm_sensor);

    // as1115_init(i2c_as1115_retro.fd, &retro_click, 2);
    // as1115_panel_clear(&retro_click);

    // max7219_init(&panel8x8_a, 0);
    // max7219_clear(&panel8x8_a);

    max7219_init(&panel8x8_b, 0);
    max7219_clear(&panel8x8_b);

    eeprom2_init(&eeprom2_logger);

    // c4x4key_init(&key4x4);

    onboard_sensors_init();

    dx_deviceTwinSubscribe(device_twin_bindings, NELEMS(device_twin_bindings));
    dx_directMethodSubscribe(direct_method_bindings, NELEMS(direct_method_bindings));

    dx_azureRegisterConnectionChangedNotification(azure_connection_state);
    dx_azureRegisterConnectionChangedNotification(startup_report);
}

static void ClosePeripheralsAndHandlers(void)
{
    dx_timerSetStop(timer_bindings, NELEMS(timer_bindings));
    dx_deviceTwinUnsubscribe();
    dx_directMethodUnsubscribe();

    dx_gpioSetClose(gpio_bindings, NELEMS(gpio_bindings));
    dx_pwmSetClose(pwm_bindings, NELEMS(pwm_bindings));
    dx_i2cSetClose(i2c_bindings, NELEMS(i2c_bindings));

    onboard_sensors_close();

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

    // run the main event loop
    dx_eventLoopRun();

    ClosePeripheralsAndHandlers();
    return dx_getTerminationExitCode();
}