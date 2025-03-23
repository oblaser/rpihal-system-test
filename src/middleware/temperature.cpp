/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "temperature.h"

#include <omw/encoding.h>
#include <rpihal/i2c.h>

#include <sys/types.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  TEMP
#include "middleware/log.h"



static RPIHAL_I2C_instance_t ___i2c;
static RPIHAL_I2C_instance_t* const i2c = &___i2c;
static uint8_t buffer[3];



static int setPointerRegister(uint8_t regAddr);



int temp::init()
{
    int err;
    ssize_t nBytes;



    err = RPIHAL_I2C_open(i2c, "/dev/i2c-1", 0x48);
    if (err)
    {
        LOG_ERR("failed to open I2C, err: %i, errno: %i %s", err, errno, std::strerror(errno));
        return -(__LINE__);
    }



    // read device ID

    if (setPointerRegister(0x0F) != 0) { return -(__LINE__); }
    nBytes = RPIHAL_I2C_read(i2c, buffer, 2);
    if (nBytes != 2)
    {
        LOG_ERR("failed to read device ID, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno));
        return -(__LINE__);
    }

    const uint16_t id = omw::bigEndian::decode_ui16(buffer);

    if (((id & 0xFF00) != 0x7500))
    {
        LOG_ERR("invalid device ID: 0x%02x", (int)id);
        return -(__LINE__);
    }



    // config 220ms continuous conversion mode

    buffer[0] = 0x01;
    buffer[1] = 0x60;
    nBytes = RPIHAL_I2C_write(i2c, buffer, 2);
    if (nBytes != 2)
    {
        LOG_ERR("failed to write config register, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno));
        return -(__LINE__);
    }



    return 0;
}

void temp::deinit()
{
    int err;

    err = RPIHAL_I2C_close(i2c);
    if (err) { LOG_ERR("failed to close I2C, err: %i, errno: %i %s", err, errno, std::strerror(errno)); }
}

float temp::get()
{
    ssize_t nBytes;

    if (setPointerRegister(0x00) != 0) { return -(__LINE__); }

    nBytes = RPIHAL_I2C_read(i2c, buffer, 2);
    if (nBytes != 2)
    {
        LOG_ERR("failed to read temperature, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno));
        return -9999;
    }

    const int16_t tempRegister = omw::bigEndian::decode_i16(buffer);
    const float temp = (float)tempRegister * 0.0625f / 16.0f;

    return temp;
}



int setPointerRegister(uint8_t regAddr)
{
    const ssize_t res = RPIHAL_I2C_write(i2c, &regAddr, 1);

    if (res != 1)
    {
        LOG_ERR("failed to set pointer register to 0x%02x, err: %lli, errno: %i %s", (int)regAddr, (long long)res, errno, std::strerror(errno));
        return -(__LINE__);
    }

    return 0;
}
