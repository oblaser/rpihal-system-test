/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "middleware/gpio.h"
#include "middleware/util.h"
#include "project.h"
#include "system-test/context.h"
#include "system-test/rpihal.h"

#include <omw/cli.h>
#include <omw/defs.h>
#include <omw/windows/windows.h>
#include <rpihal/rpihal.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_DBG
#define LOG_MODULE_NAME  MAIN
#include "middleware/log.h"


#define ARG_FLAG_TEST (0x00000001)
#define ARG_FLAG_GPIO (0x00000002)
#define ARG_FLAG_SPI  (0x00000004)
#define ARG_FLAG_I2C  (0x00000008)
#define ARG_FLAG_ALL  (ARG_FLAG_GPIO | ARG_FLAG_SPI | ARG_FLAG_I2C)
#define ARG_FLAG_APP  (0x00000010)


namespace {

enum EXITCODE // https://tldp.org/LDP/abs/html/exitcodes.html / on MSW are no preserved codes
{
    EC_OK = 0,
    EC_ERROR = 1,

    EC__begin_ = 79,

    EC_RPIHAL_INIT_ERROR = EC__begin_,
    EC_USER_ABORT,
    EC_MODEL_DETECT_FAILED,

    EC__end_,

    EC__max_ = 113
};
static_assert(EC__end_ <= EC__max_, "too many error codes defined");

}



static uint32_t parseArgs(int argc, char** argv);



int main(int argc, char** argv)
{
#if PRJ_DEBUG && 1
    {
        static char __attribute__((unused)) arg_test[] = "test";
        static char __attribute__((unused)) arg_gpio[] = "gpio";
        static char __attribute__((unused)) arg_spi[] = "spi";
        static char __attribute__((unused)) arg_i2c[] = "i2c";
        static char __attribute__((unused)) arg_all[] = "all";
        static char __attribute__((unused)) arg_app[] = "app";

        // clang-format off
        static char* ___dbg_argv[] = {
            argv[0],

            arg_test,
            arg_all,

            //arg_app,
        };
        // clang-format on

        argc = SIZEOF_ARRAY(___dbg_argv);
        argv = ___dbg_argv;
    }
#endif

#ifdef OMW_PLAT_WIN
    const auto winOutCodePage = omw::windows::consoleGetOutCodePage();
    bool winOutCodePageRes = omw::windows::consoleSetOutCodePage(omw::windows::UTF8CP);
    omw::ansiesc::enable(omw::windows::consoleEnVirtualTermProc());
#endif // OMW_PLAT_WIN

    int r = EC_OK;

    const uint32_t argFlags = parseArgs(argc, argv);


#ifdef RPIHAL_EMU
    if (RPIHAL_EMU_init(RPIHAL_model_3B) == 0)
    {
        while (!RPIHAL_EMU_isRunning()) {}
    }
#endif // RPIHAL_EMU



    if ((r == EC_OK) && !(argFlags & ARG_FLAG_TEST)) { system_test::util::printModelDetectionInfo(); }



    //==================================================================================================================
    // system test cases

    if ((r == EC_OK) && (argFlags & ARG_FLAG_TEST))
    {
        system_test::Context ctx;

        const int modelDetectErr = system_test::rpihal(ctx);

        if (modelDetectErr != 0) { r = EC_MODEL_DETECT_FAILED; }

        // if ((argFlags & ARG_FLAG_GPIO) && (modelDetectErr == 0)) { system_test::gpio(ctx); }
        // if ((argFlags & ARG_FLAG_SPI) && (modelDetectErr == 0)) { system_test::spi(ctx); }
        //  ...

        printf("\n");
        printf("test cases: %llu/%llu\n", (long long unsigned)ctx.counter().ok(), (long long unsigned)ctx.counter().total());
        printf("\n");
        printf("========================================================================\n");
        printf("\n");
    }

    // system test cases
    //==================================================================================================================
    // demo application

    if ((r == EC_OK) && (argFlags & ARG_FLAG_APP))
    {
        const int err = gpio::init();
        if (err) { r = EC_RPIHAL_INIT_ERROR; }

#if defined(PRJ_DEBUG) && 0
        RPIHAL_GPIO_dumpAltFuncReg(0x3c0000);
        RPIHAL_GPIO_dumpPullUpDnReg(0x3c0000);
#endif

        while ((r == EC_OK)
#ifdef RPIHAL_EMU                        //
               && RPIHAL_EMU_isRunning() // this is optional, but may be convenient when EMU is used a lot
#endif                                   //
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
    }

    // demo application
    //==================================================================================================================



#ifdef RPIHAL_EMU
    RPIHAL_EMU_cleanup();
#endif // RPIHAL_EMU

#ifdef OMW_PLAT_WIN
    winOutCodePageRes = omw::windows::consoleSetOutCodePage(winOutCodePage);
#endif

    return r;
}



uint32_t parseArgs(int argc, char** argv)
{
    uint32_t flags = 0;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = *(argv + i);

        if (arg == "test") { flags |= ARG_FLAG_TEST; }
        else if (arg == "gpio") { flags |= ARG_FLAG_GPIO; }
        else if (arg == "spi") { flags |= ARG_FLAG_SPI; }
        else if (arg == "i2c") { flags |= ARG_FLAG_I2C; }
        else if (arg == "all") { flags |= ARG_FLAG_ALL; }
        else if (arg == "app") { flags |= ARG_FLAG_APP; }
        else { LOG_WRN("ignoring unknown option: %s", arg.c_str()); }
    }

    return flags;
}
