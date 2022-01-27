/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

#include "hw/azure_sphere_remotex.h"

#include "dx_i2c.h"
#include "light_sensor.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#ifdef OEM_AVNET
#include "imu_temp_pressure.h"
#endif

typedef struct {
    int temperature;
    int pressure;
    int humidity;
    int co2ppm;
    int light;
} SENSOR;

typedef struct {
    SENSOR latest;
    SENSOR previous;
    bool updated;
    bool valid;
} ENVIRONMENT;

bool onboard_sensors_read(SENSOR *telemetry);
bool onboard_sensors_init(void);
bool onboard_sensors_close(void);

extern DX_I2C_BINDING i2c_onboard_sensors;