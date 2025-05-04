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



#ifdef RPIHAL_EMU
extern "C" int adc_spi_emu_transfer_callback(const uint8_t* txData, uint8_t* rxBuffer, size_t count);
#endif



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

#ifdef RPIHAL_EMU
    spi->transfer_cb = adc_spi_emu_transfer_callback;
#endif

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



#ifdef RPIHAL_EMU
extern "C" int adc_spi_emu_transfer_callback(const uint8_t* txData, uint8_t* rxBuffer, size_t count)
{
    int r = -1;

    if (count == 3)
    {
        // MCP3004 emulator

        if (txData[0] == 0x01)
        {
            const bool single = ((txData[1] & 0x80) != 0);

            const uint8_t ch = ((txData[1] >> 4) & 0x07) & /* MCP3004 ch mask */ 0x03;

            uint16_t adcResult_10bit;

            if (single)
            {
                if (ch == 0) { adcResult_10bit = 600; }
                else { adcResult_10bit = 0; }
            }
            else
            {
                adcResult_10bit = 0;
                LOG_ERR("%s differential input mode is not implemented", __func__);
            }

            // rxBuffer[0] = don't care
            rxBuffer[1] = (uint8_t)(adcResult_10bit >> 8);
            rxBuffer[2] = (uint8_t)adcResult_10bit;

            rxBuffer[1] &= ~0x04; // null bit

            r = 0;
        }
        else { LOG_ERR("%s invalid start byte: %02x", __func__, (int)(txData[0])); }
    }
    else { LOG_ERR("%s count: %zu", __func__, count); }

    return r;
}
#endif // RPIHAL_EMU
