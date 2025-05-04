/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <cstdio>
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
using std::printf;



static const char* const dev_spi = "/dev/spidev0.0";



static system_test::Case Shift_Register();
static system_test::Case Read_ADC();



system_test::Module system_test::SPI()
{
    Module module(__func__);

    module.add(Shift_Register());
    module.add(Read_ADC());

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

    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0); // prevent positive transition
    err = RPIHAL_GPIO_initPin(GPIO_SR_LATCH, &initStruct);
    CTX_REQUIRE(tc, !err, "failed to init latch pin");

    uint8_t rxBuffer[1];
    uint8_t txBuffer[1];
    RPIHAL_SPI_instance_t ___spi;
    RPIHAL_SPI_instance_t* const spi = &___spi;

    err = RPIHAL_SPI_open(spi, dev_spi, 199000, RPIHAL_SPI_CFG_MODE_0 | RPIHAL_SPI_CFG_NO_CS);
    CTX_REQUIRE(tc, !err, "failed to open SPI device " + std::string(dev_spi) + " - " + strerror(errno));



    txBuffer[0] = 0xF0;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 1);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    cli::check(tc, "LED bar 0x" + omw::toHexStr(txBuffer[0]) + "?");



    txBuffer[0] = 0xA5;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 1);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    cli::check(tc, "LED bar 0x" + omw::toHexStr(txBuffer[0]) + "?");



    txBuffer[0] = 0xD1;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 1);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    cli::check(tc, "LED bar 0x" + omw::toHexStr(txBuffer[0]) + "?");



    // clear LED bar
    txBuffer[0] = 0x00;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 1);
    RPIHAL_GPIO_writePin(GPIO_SR_LATCH, 0);
    CTX_CHECK(tc, !err, "failed to clear LED bar");



    err = RPIHAL_SPI_close(spi);
    CTX_CHECK(tc, !err, "failed to close SPI instance" + std::string(" - ") + strerror(errno));

    if (RPIHAL_GPIO_resetPin(GPIO_SR_LATCH) != 0) { system_test::cli::printWarning(tc, "failed to reset latch pin configuration"); }

    return tc;
}

system_test::Case Read_ADC()
{
    Case tc(__func__);

    int err;

    uint8_t rxBuffer[3];
    uint8_t txBuffer[3];
    RPIHAL_SPI_instance_t ___spi;
    RPIHAL_SPI_instance_t* const spi = &___spi;

    err = RPIHAL_SPI_open(spi, dev_spi, 412000, RPIHAL_SPI_CFG_MODE_3);
    CTX_REQUIRE(tc, !err, "failed to open SPI device " + std::string(dev_spi) + " - " + strerror(errno));

    std::string instr;
    uint16_t result;
    float value, target;
    constexpr float tolerance = 7.0f; // [%]

    auto getResult = [&rxBuffer]() {
        uint16_t r = (uint16_t)(rxBuffer[1] & 0x03);
        r <<= 8;
        r |= (uint16_t)(rxBuffer[2]);
        return r;
    };



    target = 50;
    instr = "turn the potentiometer to " + std::to_string((int)target) + "%";
    cli::instruct(instr);
    txBuffer[0] = 0x01;
    txBuffer[1] = 0x80;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 3);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    CTX_CHECK(tc, ((rxBuffer[1] & 0x04) == 0), "result null bit is not null");
    result = getResult();
    value = (float)result * 100.0f / 1023.0f;
    printf("%5.1f%% 0x%03x %i\n", (double)value, (int)result, (int)result);
    CTX_CHECK(tc, ((value >= (target - tolerance)) && (value <= (target + tolerance))), instr);



    target = 0;
    instr = "turn the potentiometer to " + std::to_string((int)target) + "%";
    cli::instruct(instr);
    txBuffer[0] = 0x01;
    txBuffer[1] = 0x80;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 3);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    CTX_CHECK(tc, ((rxBuffer[1] & 0x04) == 0), "result null bit is not null");
    result = getResult();
    value = (float)result * 100.0f / 1023.0f;
    printf("%5.1f%% 0x%03x %i\n", (double)value, (int)result, (int)result);
    CTX_CHECK(tc, ((value >= 0.0f) && (value <= (target + tolerance))), instr);



    target = 100;
    instr = "turn the potentiometer to " + std::to_string((int)target) + "%";
    cli::instruct(instr);
    txBuffer[0] = 0x01;
    txBuffer[1] = 0x80;
    err = RPIHAL_SPI_transfer(spi, txBuffer, rxBuffer, 3);
    CTX_CHECK(tc, !err, "RPIHAL_SPI_transfer() failed");
    CTX_CHECK(tc, ((rxBuffer[1] & 0x04) == 0), "result null bit is not null");
    result = getResult();
    value = (float)result * 100.0f / 1023.0f;
    printf("%5.1f%% 0x%03x %i\n", (double)value, (int)result, (int)result);
    CTX_CHECK(tc, ((value >= (target - tolerance)) && (value <= 100.0f)), instr);



    return tc;
}
