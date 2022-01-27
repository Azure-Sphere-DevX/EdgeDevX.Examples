/*
Simple GPIO demo - open button as input, open relay as output
*/

#include "hw/azure_sphere_remotex.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define ONE_MS 1000000

bool GetGpioState(GPIO_Value_Type *oldState);
void ClosePeripheralsAndHandlers(void);
void InitPeripheralsAndHandlers(void);

GPIO_Value_Type oldButtonState;
bool toggle_relay = false;
int fd_button = -1;
int fd_relay = -1;
int press_count = 0;

int main(int argc, char *argv[])
{
    InitPeripheralsAndHandlers();

    while (true)
    {
        if (GetGpioState(&oldButtonState))
        {
            GPIO_SetValue(fd_relay, (toggle_relay = !toggle_relay) ? GPIO_Value_High : GPIO_Value_Low);
            Log_Debug("Button pressed (%d)\n", ++press_count);
        }

        nanosleep(&(struct timespec){0, 100 * ONE_MS}, NULL);
    }

    ClosePeripheralsAndHandlers();
}

void InitPeripheralsAndHandlers(void)
{
    fd_relay = GPIO_OpenAsOutput(PIN_0, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    fd_button = GPIO_OpenAsInput(BUTTON_A);
}

void ClosePeripheralsAndHandlers(void)
{
    close(fd_relay);
    close(fd_button);
}

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