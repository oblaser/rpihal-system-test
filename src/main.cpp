/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "middleware/gpio.h"
#include "middleware/util.h"
#include "project.h"

#include <rpihal/rpihal.h>

#define LOG_MODULE_LEVEL LOG_LEVEL_DBG
#define LOG_MODULE_NAME  MAIN
#include "middleware/log.h"


namespace {

enum EXITCODE // https://tldp.org/LDP/abs/html/exitcodes.html / on MSW are no preserved codes
{
    EC_OK = 0,
    EC_ERROR = 1,

    EC__begin_ = 79,

    EC_RPIHAL_INIT_ERROR = EC__begin_,
    EC_USER_ABORT,
    // EC_..,

    EC__end_,

    EC__max_ = 113
};
static_assert(EC__end_ <= EC__max_, "too many error codes defined");

}


int main(int argc, char** argv)
{
    int r = EC_OK;
    int err;

    RPIHAL___setModel___(RPIHAL_model_3B); // temporary hack!       TODO update rpihal
#ifdef RPIHAL_EMU
    if (RPIHAL_EMU_init(RPIHAL_model_3B) == 0)
    {
        while (!RPIHAL_EMU_isRunning()) {}
    }
#endif // RPIHAL_EMU

    err = gpio::init();
    if (err) { r = EC_RPIHAL_INIT_ERROR; }

#if defined(PRJ_DEBUG) && 0
    RPIHAL_GPIO_dumpAltFuncReg(0x3c0000);
    RPIHAL_GPIO_dumpPullUpDnReg(0x3c0000);
#endif



    while (r == EC_OK)
    {
        gpio::task();

        if (gpio::btn0->pos()) { LOG_INF("BTN0 pos"); }
        if (gpio::btn0->neg()) { LOG_INF("BTN0 neg"); }

        if (gpio::btn1->pos())
        {
            LOG_INF("BTN1 pos");
            gpio::led1->toggle();
        }

        if (gpio::btn1->neg()) { LOG_INF("BTN1 neg"); }

        gpio::led0->write(gpio::btn0->state());

        util::sleep(5);
    }

    gpio::reset();

    return r;
}
