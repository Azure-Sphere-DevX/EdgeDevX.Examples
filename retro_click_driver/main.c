/*
Simple GPIO demo - open button as input, open relay as output
*/

#include "as1115.h"
#include "graphics.h"
#include "hw/azure_sphere_remotex.h"
#include <applibs/gpio.h>
#include <applibs/i2c.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define ONE_MS 1000000
#define GPIO_PIN USER_LED_RED

bool GetGpioState(GPIO_Value_Type *oldState);
void ClosePeripheralsAndHandlers(void);
void InitPeripheralsAndHandlers(void);

GPIO_Value_Type oldButtonState;
bool toggle_relay = true;
int fd_button = -1;
int fd_relay = -1;
int press_count = 0;

as1115_t retro_click = {
    .interfaceId = I2C_MASTER, .handle = -1, .bitmap64 = 0, .keymap = 0, .debouncePeriodMilliseconds = 500};

bool GetGpioState(GPIO_Value_Type *oldState)
{
    bool isGpioOn = false;
    GPIO_Value_Type newState;

    if (GPIO_GetValue(fd_button, &newState) == 0)
    {
        isGpioOn = (newState != *oldState) && (newState == GPIO_Value_Low);
        *oldState = newState;
    }
    return isGpioOn;
}

void InitPeripheralsAndHandlers(void)
{
    fd_relay = GPIO_OpenAsOutput(GPIO_PIN, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    fd_button = GPIO_OpenAsInput(BUTTON_A);
    retro_click.handle = I2CMaster_Open(retro_click.interfaceId);

    as1115_init(retro_click.handle, &retro_click, 2);
}

void ClosePeripheralsAndHandlers(void)
{
    close(fd_relay);
    close(fd_button);
}

int main(int argc, char *argv[])
{
    char letter = ' ';

    InitPeripheralsAndHandlers();

    while (true)
    {
        if (GetGpioState(&oldButtonState))
        {
            GPIO_SetValue(fd_relay, (toggle_relay = !toggle_relay) ? GPIO_Value_High : GPIO_Value_Low);
            Log_Debug("Button pressed (%d)\n", ++press_count);

            for (size_t i = 0; i < 1000000; i++)
            {
                for (char i = ' '; i <= 'z'; i++)
                {
                    gfx_load_character(i, retro_click.bitmap);

                    gfx_rotate_counterclockwise(retro_click.bitmap, 1, 1, retro_click.bitmap);
                    gfx_reverse_panel(retro_click.bitmap);
                    gfx_rotate_counterclockwise(retro_click.bitmap, 1, 1, retro_click.bitmap);

                    as1115_panel_write(&retro_click);

                    nanosleep(&(struct timespec){0, 50 * ONE_MS}, NULL);
                }
            }

            gfx_load_character(letter, retro_click.bitmap);

            gfx_rotate_counterclockwise(retro_click.bitmap, 1, 1, retro_click.bitmap);
            gfx_reverse_panel(retro_click.bitmap);
            gfx_rotate_counterclockwise(retro_click.bitmap, 1, 1, retro_click.bitmap);

            as1115_panel_write(&retro_click);

            letter = ++letter > 'z' ? ' ' : letter;
        }
        nanosleep(&(struct timespec){0, 50 * ONE_MS}, NULL);
    }

    ClosePeripheralsAndHandlers();
}