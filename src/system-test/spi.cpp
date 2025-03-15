/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "gpio-pins.h"
#include "project.h"
#include "spi.h"
#include "system-test/cli.h"
#include "system-test/context.h"

#include <rpihal/gpio.h>
#include <rpihal/spi.h>

#include <errno.h>
#include <string.h>


using namespace system_test;



// added `dtoverlay=spi0-1cs` to top of "/boot/firmware/config.txt"
static const char* const dev_spi = "/dev/spidev0.0";



static system_test::Case Shift_Register();



system_test::Module system_test::SPI()
{
    Module module(__func__);

    module.add(Shift_Register());

    return module;
}



system_test::Case Shift_Register()
{
    Case tc(__func__);

    int err;

    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);
    initStruct.mode = RPIHAL_GPIO_MODE_OUT;
    initStruct.pull = RPIHAL_GPIO_PULL_NONE;

    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0); // make sure that the pin stays low
    err = RPIHAL_GPIO_initPin(GPIO_SR_LATCH, &initStruct);
    CTX_REQUIRE(tc, !err, "failed to init latch pin");

    uint8_t rxBuffer[1];
    uint8_t txBuffer[1];
    RPIHAL_SPI_instance_t ___spi;
    RPIHAL_SPI_instance_t* const spi = &___spi;

    err = RPIHAL_SPI_open(spi, dev_spi, 199000, RPIHAL_SPI_MODE_0, RPIHAL_SPI_CFG_FLAG_NO_CS);
    CTX_REQUIRE(tc, !err, "failed to open SPI device " + std::string(dev_spi) + " - " + strerror(errno));



    txBuffer[0] = 0xD1;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 1);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    cli::check(tc, "LED bar 0xD1?");



    CTX_CHECK(tc, 0, "TODO ...");



    // clear LED bar
    txBuffer[0] = 0xD1;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    CTX_CHECK(tc, !err, "failed to clear LED bar");



    err = RPIHAL_SPI_close(spi);
    CTX_CHECK(tc, !err, "failed to close SPI instance" + std::string(" - ") + strerror(errno));

    if (RPIHAL_GPIO_resetPin(GPIO_SR_LATCH) != 0) { system_test::cli::printWarning(tc, "failed to reset latch pin configuration"); }

    return tc;
}
