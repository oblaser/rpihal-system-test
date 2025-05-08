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


#ifdef _MSC_VER // clang-format off
#pragma warning(disable: 4996)
#endif // clang-format on


#define LATCH_AS_nCS   (1) // driving the latch pin like a nCS signal, helps logic analyzers
#define MAX_CLOCK_FREQ (411000)



static RPIHAL_SPI_instance_t ___spi;
static RPIHAL_SPI_instance_t* const spi = &___spi;



#ifdef RPIHAL_EMU
extern "C" int ledbar_spi_emu_transfer_callback(const uint8_t* txData, uint8_t* rxBuffer, size_t count);
#endif



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

#ifdef RPIHAL_EMU
    spi->transfer_cb = ledbar_spi_emu_transfer_callback;
#endif

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



#ifdef RPIHAL_EMU
#include <stdio.h>
extern "C" int ledbar_spi_emu_transfer_callback(const uint8_t* txData, uint8_t* rxBuffer, size_t count)
{
    int r = -1;

    if (count == 1)
    {
        const int value = txData[0];

        const char* const format = "\033[20C"                         // cursor forward
                                   "0x%02x "                          // value
                                   "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s" // bits SGR
                                   "\033[49;39m"                      // SGR reset BG and FG
                                   "\r";                              // cursor return



#ifdef _WIN32
        const char* const lc = "\xE2\x96\xA0"; // LED char
        const char* const sgrOn = "\033[93m";
        const char* const sgrOff = "\033[90m";
#else
        const char* const lc = "\xE2\x88\x8E"; // LED char
        const char* const sgrOn = "\033[38:5:208m";
        const char* const sgrOff = "\033[38:5:237m";
#endif

        const char* const sgr_b0 = (value & 0x01 ? sgrOn : sgrOff);
        const char* const sgr_b1 = (value & 0x02 ? sgrOn : sgrOff);
        const char* const sgr_b2 = (value & 0x04 ? sgrOn : sgrOff);
        const char* const sgr_b3 = (value & 0x08 ? sgrOn : sgrOff);
        const char* const sgr_b4 = (value & 0x10 ? sgrOn : sgrOff);
        const char* const sgr_b5 = (value & 0x20 ? sgrOn : sgrOff);
        const char* const sgr_b6 = (value & 0x40 ? sgrOn : sgrOff);
        const char* const sgr_b7 = (value & 0x80 ? sgrOn : sgrOff);

        printf(format, value, sgr_b7, lc, sgr_b6, lc, sgr_b5, lc, sgr_b4, lc, sgr_b3, lc, sgr_b2, lc, sgr_b1, lc, sgr_b0, lc);

        r = 0;
    }
    else { LOG_ERR("%s count: %zu", __func__, count); }

    return r;
}
#endif // RPIHAL_EMU
