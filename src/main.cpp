/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "middleware/gpio.h"
#include "middleware/util.h"
#include "project.h"
#include "system-test/rpihal.h"

#include <omw/cli.h>
#include <omw/defs.h>
#include <omw/windows/windows.h>
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
#ifdef OMW_PLAT_WIN
    const auto winOutCodePage = omw::windows::consoleGetOutCodePage();
    bool winOutCodePageRes = omw::windows::consoleSetOutCodePage(omw::windows::UTF8CP);
    omw::ansiesc::enable(omw::windows::consoleEnVirtualTermProc());
#endif // OMW_PLAT_WIN

    int r = EC_OK;
    int err;

#ifdef RPIHAL_EMU
    if (RPIHAL_EMU_init(RPIHAL_model_3B) == 0)
    {
        while (!RPIHAL_EMU_isRunning()) {}
    }
#endif // RPIHAL_EMU



    //==================================================================================================================
    // system test cases

    if (r == EC_OK)
    {
        system_test::Context ctx;

        system_test::rpihal(ctx);

        printf("\n");
        printf("test cases: %llu/%llu\n", (long long unsigned)ctx.counter().ok(), (long long unsigned)ctx.counter().total());
        printf("\n");
        printf("========================================================================\n");
        printf("\n");
    }

    // system test cases
    //==================================================================================================================
    // normal application



    err = gpio::init();
    if (err) { r = EC_RPIHAL_INIT_ERROR; }

#if defined(PRJ_DEBUG) && 0
    RPIHAL_GPIO_dumpAltFuncReg(0x3c0000);
    RPIHAL_GPIO_dumpPullUpDnReg(0x3c0000);
#endif

    while ((r == EC_OK)
#ifdef RPIHAL_EMU                    //
           && RPIHAL_EMU_isRunning() // this is optional, but may be convenient when EMU is used a lot
#endif                               //
    )
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

#ifdef RPIHAL_EMU
    RPIHAL_EMU_cleanup();
#endif // RPIHAL_EMU

#ifdef OMW_PLAT_WIN
    winOutCodePageRes = omw::windows::consoleSetOutCodePage(winOutCodePage);
#endif

    return r;
}
