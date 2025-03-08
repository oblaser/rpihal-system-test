/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "gpio.h"

#include <rpihal/gpio.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_DBG
#define LOG_MODULE_NAME  GPIO
#include "middleware/log.h"


int gpio::init()
{
    int r = RPIHAL_GPIO_init();

    if (r == 0)
    {
        RPIHAL_GPIO_init_t initStruct;
        RPIHAL_GPIO_defaultInitStruct(&initStruct);



        initStruct.mode = RPIHAL_GPIO_MODE_IN;
        initStruct.pull = RPIHAL_GPIO_PULL_NONE;
        if (RPIHAL_GPIO_initPin(GPIO_BTN0, &initStruct)) { r = -(__LINE__); }
        if (RPIHAL_GPIO_initPin(GPIO_BTN1, &initStruct)) { r = -(__LINE__); }



        initStruct.mode = RPIHAL_GPIO_MODE_OUT;
        initStruct.pull = RPIHAL_GPIO_PULL_NONE;
        if (RPIHAL_GPIO_initPin(GPIO_LED0, &initStruct)) { r = -(__LINE__); }
        if (RPIHAL_GPIO_initPin(GPIO_LED1, &initStruct)) { r = -(__LINE__); }



        if (r) { LOG_ERR("RPIHAL_GPIO_initPin() failed at line %i", -r); }
    }
    else { LOG_ERR("RPIHAL_GPIO_init() failed %i", r); }

    return r;
}

int gpio::reset()
{
    int r = 0;

    if (RPIHAL_GPIO_resetPin(GPIO_BTN0)) { r = -(__LINE__); }
    if (RPIHAL_GPIO_resetPin(GPIO_BTN1)) { r = -(__LINE__); }

    if (RPIHAL_GPIO_resetPin(GPIO_LED0)) { r = -(__LINE__); }
    if (RPIHAL_GPIO_resetPin(GPIO_LED1)) { r = -(__LINE__); }

    if (r) { LOG_ERR("RPIHAL_GPIO_resetPin() failed at line %i", -r); }

    return r;
}

void gpio::task()
{
    btn0->handler();
    btn1->handler();
}



static gpio::InputActiveHigh ___btn0(GPIO_BTN0);
static gpio::InputActiveHigh ___btn1(GPIO_BTN1);

static gpio::OutputActiveHigh ___led0(GPIO_LED0);
static gpio::OutputActiveHigh ___led1(GPIO_LED1);



namespace gpio {

Input* const btn0 = &___btn0;
Input* const btn1 = &___btn1;

Output* const led0 = &___led0;
Output* const led1 = &___led1;

} // namespace gpio
