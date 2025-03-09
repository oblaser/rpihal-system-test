/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "gpio.h"
#include "middleware/gpio.h" // only for GPIO numbers defines
#include "project.h"
#include "system-test/cli.h"
#include "system-test/context.h"

#include <omw/string.h>
#include <rpihal/gpio.h>


using namespace system_test;



static system_test::Case PullUp_PullDown();

static void initGpioPin(system_test::TestObejct& to, int pin, const RPIHAL_GPIO_init_t* initStruct) noexcept(false);
static void initGpioPins(system_test::TestObejct& to, uint64_t bits, const RPIHAL_GPIO_init_t* initStruct) noexcept(false);
static void resetGpioPins(system_test::TestObejct& to);



system_test::TestObejct system_test::GPIO_init()
{
    system_test::TestObejct r("RPIHAL_GPIO_init()");

    if (RPIHAL_GPIO_init() != 0) { cli::printError(r, "RPIHAL_GPIO_init() failed"); }

    return r;
}

system_test::Module system_test::GPIO()
{
    Module module(__func__);

    module.add(PullUp_PullDown());

    resetGpioPins(module);

    return module;
}



system_test::Case PullUp_PullDown()
{
    Case tc(__func__);

    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);
    initStruct.mode = RPIHAL_GPIO_MODE_IN;

    const uint64_t ledPinsBits = (RPIHAL_GPIO_pintobit(GPIO_LED0) | RPIHAL_GPIO_pintobit(GPIO_LED1));

    try
    {
        initStruct.pull = RPIHAL_GPIO_PULL_UP;
        initGpioPins(tc, ledPinsBits, &initStruct);

        cli::check(tc, "is LED0 and LED1 on?");
    }
    catch (...)
    {}

    try
    {
        initStruct.pull = RPIHAL_GPIO_PULL_DOWN;
        initGpioPins(tc, ledPinsBits, &initStruct);

        cli::check(tc, "is LED0 and LED1 off?");
    }
    catch (...)
    {}

    try
    {
        initStruct.pull = RPIHAL_GPIO_PULL_DOWN;
        initGpioPin(tc, GPIO_LED0, &initStruct);

        initStruct.pull = RPIHAL_GPIO_PULL_UP;
        initGpioPin(tc, GPIO_LED1, &initStruct);

        cli::check(tc, "is LED0 off and LED1 on?");
    }
    catch (...)
    {}

    try
    {
        initStruct.pull = RPIHAL_GPIO_PULL_UP;
        initGpioPin(tc, GPIO_LED0, &initStruct);

        initStruct.pull = RPIHAL_GPIO_PULL_DOWN;
        initGpioPin(tc, GPIO_LED1, &initStruct);

        cli::check(tc, "is LED0 on and LED1 off?");
    }
    catch (...)
    {}

    return tc;
}



void initGpioPin(system_test::TestObejct& to, int pin, const RPIHAL_GPIO_init_t* initStruct) noexcept(false)
{
    const int err = RPIHAL_GPIO_initPin(pin, initStruct);

    if (err)
    {
        const auto msg = "failed to init pin " + std::to_string(pin);
        system_test::cli::printError(to, msg);
        throw -(__LINE__);
    }
}

void initGpioPins(system_test::TestObejct& to, uint64_t bits, const RPIHAL_GPIO_init_t* initStruct) noexcept(false)
{
    const int err = RPIHAL_GPIO_initPins(bits, initStruct);

    if (err)
    {
        const auto msg = "failed to init pins, mask: 0x" + omw::toHexStr(bits);
        system_test::cli::printError(to, msg);
        throw -(__LINE__);
    }
}

void resetGpioPins(system_test::TestObejct& to)
{
    if (RPIHAL_GPIO_resetPin(GPIO_BTN0) != 0) { system_test::cli::printWarning(to, "failed to reset BTN0 pin configuration"); }
    if (RPIHAL_GPIO_resetPin(GPIO_BTN1) != 0) { system_test::cli::printWarning(to, "failed to reset BTN1 pin configuration"); }
    if (RPIHAL_GPIO_resetPin(GPIO_LED0) != 0) { system_test::cli::printWarning(to, "failed to reset LED0 pin configuration"); }
    if (RPIHAL_GPIO_resetPin(GPIO_LED1) != 0) { system_test::cli::printWarning(to, "failed to reset LED1 pin configuration"); }
}
