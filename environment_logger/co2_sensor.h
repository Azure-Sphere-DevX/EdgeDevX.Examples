/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#pragma once

#include <stdbool.h>
#include <applibs/log.h>

typedef struct {
    int temperature;
    int pressure;
    int humidity;
    int co2ppm;
    int light;
} SENSOR_T;

typedef struct {
    SENSOR_T latest;
    SENSOR_T previous;
    bool updated;
    bool valid;
} ENVIRONMENT_T;

#ifdef SCD30
#include "AzureSphereDrivers/EmbeddedScd30/scd30/scd30.h"
#else
#include "scd4x_i2c.h"
#include "sensirion_i2c_hal.h"
#include "sensirion_common.h"
#endif

/// <summary>
/// Initialize the CO2 sensor
/// </summary>
/// <param name=""></param>
/// <returns></returns>
bool co2_initialize(int fd);

/// <summary>
/// Read CO2 sensor telemetry
/// </summary>
/// <param name="telemetry"></param>
/// <returns></returns>
bool co2_read(ENVIRONMENT_T *telemetry);

/// <summary>
/// Set the CO2 sensor altitude
/// </summary>
/// <param name="altitude_in_meters"></param>
/// <returns></returns>
bool co2_set_altitude(int altitude_in_meters);