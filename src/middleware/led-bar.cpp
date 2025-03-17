/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "gpio-pins.h"
#include "led-bar.h"

#include <rpihal/gpio.h>
#include <rpihal/spi.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  LEDBAR
#include "middleware/log.h"


#define LATCH_AS_nCS   (1) // driving the latch pin like a nCS signal, helps logic analyzers
#define MAX_CLOCK_FREQ (411000)



static RPIHAL_SPI_instance_t ___spi;
static RPIHAL_SPI_instance_t* const spi = &___spi;



int ledBar::init()
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
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0); // prevent positive transition
    err = RPIHAL_GPIO_initPin(GPIO_SR_LATCH, &initStruct);
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

void ledBar::deinit()
{
    int err;

    err = RPIHAL_GPIO_resetPin(GPIO_SR_LATCH);
    if (err) { LOG_ERR("failed to deinit latch pin: %i", err); }

    err = RPIHAL_SPI_close(spi);
    if (err) { LOG_ERR("failed to close SPI, err: %i, errno: %i %s", err, errno, std::strerror(errno)); }
}

void ledBar::setBar(int value)
{
    const uint8_t tmp = (uint8_t)((1 << (value % 9)) - 1);

    LOG_DBG("%i %i, %02x", value, (value % 9), (int)tmp);

    setValue(tmp);
}

void ledBar::setValue(uint8_t value)
{
    uint8_t rxDummy[1];

#if LATCH_AS_nCS
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
#endif // LATCH_AS_nCS

    const int err = RPIHAL_SPI_transfer(spi, &value, rxDummy, 1);

    if (err) { LOG_ERR("failed to set value, err: %i, errno: %i %s", err, errno, std::strerror(errno)); }
#if !LATCH_AS_nCS
    else
    {
        // t_i >= 400ns
        RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
        volatile int i = 0;
        while (i < 1) { ++i; }
        RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    }
#else  // LATCH_AS_nCS
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
#endif // LATCH_AS_nCS
}
