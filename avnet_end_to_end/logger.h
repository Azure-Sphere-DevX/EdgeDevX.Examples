#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <applibs/remotex.h>
#include <applibs/storage.h>
#include "dx_utilities.h"
#include <unistd.h>

bool log_open(uint16_t logger_block_size);
bool log_read(uint8_t *data, size_t data_length, uint16_t block_id);
bool log_write(uint8_t *data, size_t data_length, uint16_t block_id);