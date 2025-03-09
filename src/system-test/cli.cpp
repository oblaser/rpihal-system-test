/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>

#include "cli.h"
#include "middleware/util.h"
#include "project.h"
#include "system-test/context.h"

#include <omw/cli.h>
#include <omw/string.h>


using std::printf;

namespace {

constexpr int ewiWidth = 10;

}



void system_test::cli::printModuleTitle(const std::string& moduleName__func__)
{
    printf(CLI_SGR_GREEN "  --======# " CLI_SGR_BGREEN "%s" CLI_SGR_GREEN " #======--" CLI_SGR_FG_DEFAULT "\n", moduleName__func__.c_str());
}

void system_test::cli::printTestCaseTitle(const std::string& caseName__func__)
{
    std::string testCaseTitle(caseName__func__);
    omw::replaceAll(testCaseTitle, '_', ' ');

    printf(CLI_SGR_CYAN "  --======# " CLI_SGR_BCYAN "%s" CLI_SGR_CYAN " #======--" CLI_SGR_FG_DEFAULT "\n", testCaseTitle.c_str());
}

void system_test::cli::printUnassocTitle(const std::string& name)
{
    printf(CLI_SGR_BBLACK "  --======# " CLI_SGR_WHITE "%s" CLI_SGR_BBLACK " #======--" CLI_SGR_FG_DEFAULT "\n", name.c_str());
}

void system_test::cli::printResult(const system_test::Context& ctx)
{
    printf("\n");

    const unsigned long long ok = ctx.counter().ok();
    const unsigned long long total = ctx.counter().total();

    if (ctx.allOk())
    {
        printf(CLI_SGR_BGREEN "===============================================================================" CLI_SGR_FG_DEFAULT "\n");
        printf(CLI_SGR_BGREEN "OK" CLI_SGR_FG_DEFAULT "   %llu assertions\n", total);
    }
    else
    {
        constexpr size_t n = 79;
        const float ratio = (float)ok / (float)total;

        size_t nRed = n - (size_t)((float)n * ratio + 0.5f);

        // override rounding so that at least one red or green is shown
        if (nRed == 0) { nRed = 1; }
        if ((nRed == n) && (ok != 0)) { nRed = n - 1; }

        // assemble the bar
        std::string bar = CLI_SGR_BRED;
        for (size_t i = 0; i < n; ++i)
        {
            if (i == nRed) { bar += CLI_SGR_GREEN; }
            bar += '=';
        }
        bar += CLI_SGR_FG_DEFAULT;

        printf("%s\n", bar.c_str());
        printf(CLI_SGR_BRED "failed" CLI_SGR_FG_DEFAULT "   %llu/%llu passed\n", ok, total);
    }

    printf("\n");



    /* catch2 OK result:

    ===============================================================================
    All tests passed (6786 assertions in 127 test cases)



    \033[92m===============================================================================\033[39m
    \033[92mAll tests passed\033[39m (6786 assertions in 127 test cases)

    */

    /* catch2 failed result:

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    omw-unit-test-static is a Catch v2.13.7 host application.
    Run with -? for options

    -------------------------------------------------------------------------------
    int.h macros
    -------------------------------------------------------------------------------
    /home/jon/prj/rpihal-system-test/sdk/omw/tests/unit/int.hpp:26
    ...............................................................................

    /home/jon/prj/rpihal-system-test/sdk/omw/tests/unit/int.hpp:36: FAILED:
    CHECK( false )

    -------------------------------------------------------------------------------
    vector.h omw::vector ctor
    -------------------------------------------------------------------------------
    /home/jon/prj/rpihal-system-test/sdk/omw/tests/unit/vector.hpp:36
    ...............................................................................

    /home/jon/prj/rpihal-system-test/sdk/omw/tests/unit/vector.hpp:43: FAILED:
    CHECK( false )

    ===============================================================================
    test cases:  127 |  125 passed | 2 failed
    assertions: 6788 | 6786 passed | 2 failed



    \033[91m=\033[32m==============================================================================\033[39m
    test cases:  127 | \033[32m 125 passed\033[39m | \033[91m2 failed\033[39m
    assertions: 6788 | \033[32m6786 passed\033[39m | \033[91m2 failed\033[39m

    */
}

void system_test::cli::printError(system_test::TestObejct& to, const std::string& text)
{
    to.counter().add(1, 0);

    std::cout << omw::fgBrightRed << std::left << std::setw(ewiWidth) << "error:" << omw::defaultForeColor;
    std::cout << text; // printFormattedText(text);
    std::cout << std::endl;
}

void system_test::cli::printWarning(system_test::TestObejct& to, const std::string& text)
{
    std::cout << omw::fgBrightYellow << std::left << std::setw(ewiWidth) << "warning:" << omw::defaultForeColor;
    std::cout << text; // printFormattedText(text);
    std::cout << std::endl;
}

void system_test::cli::instruct(const std::string& text)
{
    bool done = false;
    std::string data;

    do {
        std::cout << CLI_SGR_BWHITE << text << CLI_SGR_FG_DEFAULT << " [enter] " << std::flush;
        std::getline(std::cin, data);

        if (data.length() == 0) { done = true; }
    }
    while (!done);
}

bool system_test::cli::check(system_test::TestObejct& to, const std::string& text)
{
    bool r;

    const auto ans = omw_::cli::choice(CLI_SGR_BWHITE + text + CLI_SGR_FG_DEFAULT, omw_::cli::ChoiceAnswer::none, 'y', 'n');

    if (ans == omw_::cli::ChoiceAnswer::A)
    {
        to.counter().add(1, 1);
        r = true;
    }
    else
    {
        to.counter().add(1, 0);
        r = false;
    }

    return r;
}
