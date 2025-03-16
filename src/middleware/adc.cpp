/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "adc.h"

#include <rpihal/spi.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  ADC
#include "middleware/log.h"



static RPIHAL_SPI_instance_t ___spi;
static RPIHAL_SPI_instance_t* const spi = &___spi;



int adc::init()
{
    int err;

    err = RPIHAL_SPI_open(spi, "/dev/spidev0.0", 811000, RPIHAL_SPI_MODE_3, RPIHAL_SPI_CFG_FLAG_DEFAULT);
    if (err)
    {
        LOG_ERR("failed to open SPI, err: %i, errno: %i %s", err, errno, std::strerror(errno));
        return -(__LINE__);
    }

    return 0;
}

void adc::deinit()
{
    int err;

    err = RPIHAL_SPI_close(spi);
    if (err) { LOG_ERR("failed to close SPI, err: %i, errno: %i %s", err, errno, std::strerror(errno)); }
}

adc::Result adc::read(uint8_t channel)
{
    Result r(0);

    int err;

    uint8_t rxBuffer[3];
    uint8_t txBuffer[3];

    txBuffer[0] = 0x01;
    txBuffer[1] = (0x80 | ((channel & 0x03) << 4));

    LOG_DBG("ch: %i, tx[1]: 0x%02x", (int)channel, (int)(txBuffer[1]));

    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 3);

    if (err) { LOG_ERR("failed to read channel, err: %i, errno: %i %s", err, errno, std::strerror(errno)); }
    else
    {
        uint16_t value = (uint16_t)(rxBuffer[1] & 0x03);
        value <<= 8;
        value |= (uint16_t)(rxBuffer[2]);

        r = value;

        LOG_DBG("r.value: 0x%03x %4i, r.norm: %5.1f", (int)r.value(), (int)r.value(), r.norm());
    }

    return r;
}
