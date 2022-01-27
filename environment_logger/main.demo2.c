#include "graphics.h"
#include "hw/azure_sphere_remotex.h"
#include "imu_temp_pressure.h"
#include "max7219.h"
#include "rgb_lcd.h"
#include <applibs/gpio.h>
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "logger.h"
#include "co2_sensor.h"

int open_i2c(int interfaceId, uint32_t speedInHz);
void ClosePeripheralsAndHandlers(void);
void InitPeripheralsAndHandlers(void);
void update_lcd_panel(int counter);
void update_panel8x8(int i);

typedef struct
{
    int temperature;
    int pressure;
} SENSOR_T;

ENVIRONMENT environment;

int led_fd = -1;
int hm_fd = -1;
int onboard_sensors_fd = -1;
int co2_fd = -1;

bool toggle_led = false;
char lcd_panel_text[40];

max7219_t panel8x8_b = {.interfaceId = SPI_MASTER, .chipSelectId = MT3620_SPI_CS_B, .busSpeed = 10000000, .bitmap = {0}};
rgb_lcd_16x2_t lcd_panel;

int main(int argc, char *argv[])
{
    int counter = 0;
    InitPeripheralsAndHandlers();

    while (true)
    {
        update_lcd_panel(counter);
        update_panel8x8(counter);

        GPIO_SetValue(led_fd, (toggle_led = !toggle_led) ? GPIO_Value_High : GPIO_Value_Low);

        counter++;

        nanosleep(&(struct timespec){0, 500 * 1000000}, NULL);
    }

    GPIO_SetValue(led_fd, GPIO_Value_Low);
}

void InitPeripheralsAndHandlers(void)
{
    led_fd = GPIO_OpenAsOutput(PIN_0, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    lcd_panel.fd = open_i2c(I2C_MASTER, I2C_BUS_SPEED_FAST_PLUS);
    onboard_sensors_fd = open_i2c(I2C_MASTER, I2C_BUS_SPEED_FAST_PLUS);
    co2_fd = open_i2c(I2C_MASTER, I2C_BUS_SPEED_FAST_PLUS);

    co2_initialize(co2_fd);

    log_open(sizeof(SENSOR_T));

    avnet_imu_initialize(onboard_sensors_fd);
    RGBLCD_Init(&lcd_panel);

    max7219_init(&panel8x8_b, 2);
    max7219_clear(&panel8x8_b);
}

void ClosePeripheralsAndHandlers(void)
{
    close(led_fd);
    close(lcd_panel.fd);
    close(onboard_sensors_fd);
}

void update_lcd_panel(int counter)
{

    SENSOR_T data;

    if (counter % 30 != 0)
    {
        return;
    }

    static int last_temperature = INT32_MAX, last_pressure = INT32_MAX, last_CO2_ppm = INT32_MAX;
    data.temperature = (int)avnet_get_temperature();
    data.pressure = (int)avnet_get_pressure();

    co2_read(&environment);

    // log_write((uint8_t *)&data, sizeof(SENSOR_T), counter);

    // data.temperature = data.pressure = 0;

    // log_read((uint8_t *)&data, sizeof(SENSOR_T), counter);

    if (last_temperature == environment.latest.temperature && 
        last_CO2_ppm == environment.latest.co2ppm)
    {
        return;
    }

    last_temperature = environment.latest.temperature;
    last_pressure = data.pressure;
    last_CO2_ppm = environment.latest.co2ppm;

    memset(lcd_panel_text, 0x00, sizeof(lcd_panel_text));
    snprintf(lcd_panel_text, sizeof(lcd_panel_text), "Celsius: %d\nCO2 ppm: %d", last_temperature, last_CO2_ppm);
    RGBLCD_SetText(&lcd_panel, lcd_panel_text);
}

void update_panel8x8(int i)
{
    gfx_load_character((uint8_t)((i % 10) + 48), panel8x8_b.bitmap);
    gfx_reverse_panel(panel8x8_b.bitmap);
    gfx_rotate_counterclockwise(panel8x8_b.bitmap, 1, 1, panel8x8_b.bitmap);
    gfx_reverse_panel(panel8x8_b.bitmap);
    max7219_panel_write(&panel8x8_b);
}

int open_i2c(int interfaceId, uint32_t speedInHz)
{
    hm_fd = I2CMaster_Open(interfaceId);
    if (hm_fd < 0)
    {
        Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    int result = I2CMaster_SetBusSpeed(hm_fd, I2C_BUS_SPEED_FAST_PLUS);
    if (result != 0)
    {
        Log_Debug("ERROR: I2CMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    result = I2CMaster_SetTimeout(hm_fd, 100);
    if (result != 0)
    {
        Log_Debug("ERROR: I2CMaster_SetTimeout: errno=%d (%s)\n", errno, strerror(errno));
        return -1;
    }

    return hm_fd;
}