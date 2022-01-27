#include "logger.h"

static int block_size = 128;
static int logger_fd = -1;
static bool initialized = false;

bool log_open(uint16_t logger_block_size)
{
    if (initialized)
    {
        return true;
    }

    block_size = logger_block_size;
    if ((logger_fd = Storage_OpenMutableFile()) == -1)
    {
        return false;
    }
    initialized = true;

    return true;
}

bool log_write(uint8_t *data, size_t data_length, uint16_t block_id)
{
    if (!initialized)
    {
        return false;
    }

    uint8_t logger_buffer[128];

    lseek(logger_fd, block_id * block_size, SEEK_SET);

    memset(logger_buffer, 0x00, sizeof(logger_buffer));
    memcpy(logger_buffer, data, data_length < sizeof(logger_buffer) ? data_length : sizeof(logger_buffer));

    if (write(logger_fd, logger_buffer, sizeof(logger_buffer)) != sizeof(logger_buffer))
    {
        dx_Log_Debug("Logger write failed for block id: %d\n", block_id);
        return false;
    }

    return true;
}

bool log_read(uint8_t *data, size_t data_length, uint16_t block_id)
{

    if (!initialized || data_length < block_size)
    {
        return false;
    }

    lseek(logger_fd, block_id * block_size, SEEK_SET);
    if (read(logger_fd, data, block_size) != block_size)
    {
        dx_Log_Debug("Log read failed\n");
        return false;
    }
    return true;
}