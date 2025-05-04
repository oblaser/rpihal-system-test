/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "gpio-pins.h"
#include "gpio.h"
#include "project.h"
#include "system-test/cli.h"
#include "system-test/context.h"

#include <omw/string.h>

#include <rpihal/gpio.h>


using namespace system_test;



static system_test::Case PullUp_PullDown();
static system_test::Case Input();
static system_test::Case Output();

static void initGpioPin(system_test::TestObejct& to, int pin, const RPIHAL_GPIO_init_t* initStruct) noexcept(false);
static void initGpioPins(system_test::TestObejct& to, uint64_t bits, const RPIHAL_GPIO_init_t* initStruct) noexcept(false);
static void assertGpioRead(system_test::Case& tc, uint64_t mask, uint64_t expectedValue);
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
    module.add(Input());
    module.add(Output());

    resetGpioPins(module);

    return module;
}



system_test::Case PullUp_PullDown()
{
    Case tc(__func__);

    constexpr uint64_t pinBits = (RPIHAL_GPIO_BIT(GPIO_LED0) | RPIHAL_GPIO_BIT(GPIO_LED1));
    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);
    initStruct.mode = RPIHAL_GPIO_MODE_IN;



    try
    {
        initStruct.pull = RPIHAL_GPIO_PULL_UP;
        initGpioPins(tc, pinBits, &initStruct);

        cli::check(tc, "is LED0 and LED1 on?");
    }
    catch (...)
    {}

    try
    {
        initStruct.pull = RPIHAL_GPIO_PULL_DOWN;
        initGpioPins(tc, pinBits, &initStruct);

        cli::check(tc, "is LED0 and LED1 off?");
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



    RPIHAL_GPIO_resetPin(GPIO_LED0);
    RPIHAL_GPIO_resetPin(GPIO_LED1);

    return tc;
}

system_test::Case Input()
{
    Case tc(__func__);

    constexpr uint64_t pinBits = (RPIHAL_GPIO_BIT(GPIO_BTN0) | RPIHAL_GPIO_BIT(GPIO_BTN1));
    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);

    initStruct.mode = RPIHAL_GPIO_MODE_IN;
    initStruct.pull = RPIHAL_GPIO_PULL_NONE;

    // ignore result, already prints warnings and errors, and buttons are not directly connected
    RPIHAL_GPIO_initPins(pinBits, &initStruct);

    if (RPIHAL_GPIO_readPin(GPIO_BTN0) > 0) { cli::printWarning(tc, "BTN0 is pressed"); }
    if (RPIHAL_GPIO_readPin(GPIO_BTN1) > 0) { cli::printWarning(tc, "BTN1 is pressed"); }



    cli::instruct("press and hold BTN0");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN0) == 1, "BTN0 is not high");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN1) == 0, "BTN1 is not low");
    assertGpioRead(tc, pinBits, RPIHAL_GPIO_BIT(GPIO_BTN0));

    cli::instruct("press and hold BTN1");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN0) == 0, "BTN0 is not low");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN1) == 1, "BTN1 is not high");
    assertGpioRead(tc, pinBits, RPIHAL_GPIO_BIT(GPIO_BTN1));

    cli::instruct("press and hold BTN0 and BTN1");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN0) == 1, "BTN0 is not high");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN1) == 1, "BTN1 is not high");
    assertGpioRead(tc, pinBits, pinBits);

    cli::instruct("release BTN0 and BTN1");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN0) == 0, "BTN0 is not low");
    tc.assert(RPIHAL_GPIO_readPin(GPIO_BTN1) == 0, "BTN1 is not low");
    assertGpioRead(tc, pinBits, 0);



    RPIHAL_GPIO_resetPin(GPIO_BTN0);
    RPIHAL_GPIO_resetPin(GPIO_BTN1);

    return tc;
}

system_test::Case Output()
{
    Case tc(__func__);

    constexpr uint64_t pinBits = (RPIHAL_GPIO_BIT(GPIO_LED0) | RPIHAL_GPIO_BIT(GPIO_LED1));
    RPIHAL_GPIO_init_t initStruct;
    RPIHAL_GPIO_defaultInitStruct(&initStruct);
    initStruct.mode = RPIHAL_GPIO_MODE_OUT;
    initStruct.pull = RPIHAL_GPIO_PULL_NONE;

    CTX_REQUIRE(tc, (RPIHAL_GPIO_initPins(pinBits, &initStruct) == 0), "failed to init pins");



    RPIHAL_GPIO_writePin(GPIO_LED0, 1);
    RPIHAL_GPIO_writePin(GPIO_LED1, 1);
    cli::check(tc, "is LED0 and LED1 on?");

    RPIHAL_GPIO_writePin(GPIO_LED0, 0);
    RPIHAL_GPIO_writePin(GPIO_LED1, 0);
    cli::check(tc, "is LED0 and LED1 off?");

    RPIHAL_GPIO_set(RPIHAL_GPIO_BIT(GPIO_LED0));
    RPIHAL_GPIO_clr(RPIHAL_GPIO_BIT(GPIO_LED1));
    cli::check(tc, "is LED0 on and LED1 off?");

    RPIHAL_GPIO_clr(RPIHAL_GPIO_BIT(GPIO_LED0));
    RPIHAL_GPIO_set(RPIHAL_GPIO_BIT(GPIO_LED1));
    cli::check(tc, "is LED0 off and LED1 on?");

    RPIHAL_GPIO_togglePin(GPIO_LED0);
    RPIHAL_GPIO_togglePin(GPIO_LED1);
    cli::check(tc, "is LED0 on and LED1 off? (toggle)");



    RPIHAL_GPIO_resetPin(GPIO_LED0);
    RPIHAL_GPIO_resetPin(GPIO_LED1);

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

void assertGpioRead(system_test::Case& tc, uint64_t mask, uint64_t expectedValue)
{
    const uint32_t valueLo = RPIHAL_GPIO_read();
    const uint32_t valueHi = RPIHAL_GPIO_readHi();
    const uint64_t value = RPIHAL_GPIO_read64();

    const uint32_t maskLo = (uint32_t)(mask);
    const uint32_t maskHi = (uint32_t)(mask >> 32);

    const uint32_t expectedValueLo = (uint32_t)(expectedValue);
    const uint32_t expectedValueHi = (uint32_t)(expectedValue >> 32);



    CTX_CHECK(tc, ((valueLo & maskLo) == expectedValueLo),
              "RPIHAL_GPIO_read() returned " + omw::toHexStr(valueLo) + ", mask: " + omw::toHexStr(maskLo) + ", expected: " + omw::toHexStr(expectedValueLo));

    CTX_CHECK(tc, ((valueHi & maskHi) == expectedValueHi),
              "RPIHAL_GPIO_readHi() returned " + omw::toHexStr(valueHi) + ", mask: " + omw::toHexStr(maskHi) + ", expected: " + omw::toHexStr(expectedValueHi));

    CTX_CHECK(tc, ((value & mask) == expectedValue),
              "RPIHAL_GPIO_read64() returned " + omw::toHexStr(value) + ", mask: " + omw::toHexStr(mask) + ", expected: " + omw::toHexStr(expectedValue));
}

void resetGpioPins(system_test::TestObejct& to)
{
    if (RPIHAL_GPIO_resetPin(GPIO_BTN0) != 0) { system_test::cli::printWarning(to, "failed to reset BTN0 pin configuration"); }
    if (RPIHAL_GPIO_resetPin(GPIO_BTN1) != 0) { system_test::cli::printWarning(to, "failed to reset BTN1 pin configuration"); }
    if (RPIHAL_GPIO_resetPin(GPIO_LED0) != 0) { system_test::cli::printWarning(to, "failed to reset LED0 pin configuration"); }
    if (RPIHAL_GPIO_resetPin(GPIO_LED1) != 0) { system_test::cli::printWarning(to, "failed to reset LED1 pin configuration"); }
}
