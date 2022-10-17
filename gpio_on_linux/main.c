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

#include <stdio.h>
#include "dx_gpio.h"
#include <time.h>

#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

static DX_GPIO_BINDING gpio_led_red = {.chip_number = 3, .line_number = 19, .direction = DX_GPIO_OUTPUT, .name = "button_1"};
static DX_GPIO_BINDING gpio_led_green = {.chip_number = 3, .line_number = 21, .direction = DX_GPIO_OUTPUT, .name = "led_green"};
static DX_GPIO_BINDING gpio_led_blue = {.chip_number = 1, .line_number = 18, .direction = DX_GPIO_OUTPUT, .name = "led_blue"};
static DX_GPIO_BINDING gpio_button = {.chip_number = 1, .line_number = 19, .direction = DX_GPIO_INPUT, .name = "led_red"};

static DX_GPIO_BINDING *gpio_bindings[] = {&gpio_led_red, &gpio_led_green, &gpio_led_blue, &gpio_button};

int main()
{
    dx_gpioSetOpen(gpio_bindings, NELEMS(gpio_bindings));

    for (size_t i = 0; i < 1000000000; i++)
    {

        dx_gpioStateSet(&gpio_led_green, i % 2);
        dx_gpioStateSet(&gpio_led_red, i % 2);

        dx_gpioStateGet(&gpio_button) ? dx_gpioOff(&gpio_led_blue) : dx_gpioStateSet(&gpio_led_blue, i % 2);

        nanosleep(&(struct timespec){0, 100 * 1000000}, NULL);
    }

    dx_gpioSetClose(gpio_bindings, NELEMS(gpio_bindings));
}