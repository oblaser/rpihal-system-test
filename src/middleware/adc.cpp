/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "adc.h"

#include <rpihal/gpio.h>
#include <rpihal/spi.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  ADC
#include "middleware/log.h"


#define MAX_CLOCK_FREQ (811000)

#define PIN_nCS (8)



static RPIHAL_SPI_instance_t ___spi;
static RPIHAL_SPI_instance_t* const spi = &___spi;



int adc::init()
{
    int err;

    err = RPIHAL_GPIO_init();
    if (err)
    {
        LOG_ERR("RPIHAL_GPIO_init() failed: %i", err);
        return -(__LINE__);
    }

    RPIHAL_GPIO_init_t initStruct;
    initStruct.mode = RPIHAL_GPIO_MODE_OUT;
    initStruct.pull = RPIHAL_GPIO_PULL_NONE;
    RPIHAL_GPIO_writePin(PIN_nCS, 1);
    err = RPIHAL_GPIO_initPin(PIN_nCS, &initStruct);
    if (err)
    {
        LOG_ERR("failed to init latch pin: %i", err);
        return -(__LINE__);
    }

    err = RPIHAL_SPI_open(spi, "/dev/spidev0.0", MAX_CLOCK_FREQ, RPIHAL_SPI_CFG_MODE_0 | RPIHAL_SPI_CFG_NO_CS);
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

    err = RPIHAL_GPIO_resetPin(PIN_nCS);
    if (err) { LOG_ERR("failed to deinit latch pin: %i", err); }

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

    RPIHAL_GPIO_writePin(PIN_nCS, 0);
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 3);
    RPIHAL_GPIO_writePin(PIN_nCS, 1);

    if (err)
    {
        LOG_DBG("ch: %i, tx[1]: 0x%02x", (int)channel, (int)(txBuffer[1]));
        LOG_ERR("failed to read channel, err: %i, errno: %i %s", err, errno, std::strerror(errno));
    }
    else
    {
        uint16_t value = (uint16_t)(rxBuffer[1] & 0x03);
        value <<= 8;
        value |= (uint16_t)(rxBuffer[2]);

        r = value;

        LOG_DBG("ch: %i, tx[1]: 0x%02x, r.value: 0x%03x %4i, r.norm: %5.1f", (int)channel, (int)(txBuffer[1]), (int)r.value(), (int)r.value(), r.norm());
    }

    return r;
}
