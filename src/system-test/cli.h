/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_SYSTEMTEST_CLI_H
#define IG_SYSTEMTEST_CLI_H

#include <cstddef>
#include <cstdint>
#include <string>

#include "system-test/context.h"


#define CLI_SGR_RESET       "\033[0m"
#define CLI_SGR_BG_BLACK    "\033[40m"
#define CLI_SGR_BG_RED      "\033[41m"
#define CLI_SGR_BG_GREEN    "\033[42m"
#define CLI_SGR_BG_YELLOW   "\033[43m"
#define CLI_SGR_BG_BLUE     "\033[44m"
#define CLI_SGR_BG_MAGENTA  "\033[45m"
#define CLI_SGR_BG_CYAN     "\033[46m"
#define CLI_SGR_BG_WHITE    "\033[47m"
#define CLI_SGR_BG_DEFAULT  "\033[49m"
#define CLI_SGR_BG_BBLACK   "\033[100m"
#define CLI_SGR_BG_BRED     "\033[101m"
#define CLI_SGR_BG_BGREEN   "\033[102m"
#define CLI_SGR_BG_BYELLOW  "\033[103m"
#define CLI_SGR_BG_BBLUE    "\033[104m"
#define CLI_SGR_BG_BMAGENTA "\033[105m"
#define CLI_SGR_BG_BCYAN    "\033[106m"
#define CLI_SGR_BG_BWHITE   "\033[107m"

// SGR foreground colors
#define CLI_SGR_BLACK      "\033[30m"
#define CLI_SGR_RED        "\033[31m"
#define CLI_SGR_GREEN      "\033[32m"
#define CLI_SGR_YELLOW     "\033[33m"
#define CLI_SGR_BLUE       "\033[34m"
#define CLI_SGR_MAGENTA    "\033[35m"
#define CLI_SGR_CYAN       "\033[36m"
#define CLI_SGR_WHITE      "\033[37m"
#define CLI_SGR_FG_DEFAULT "\033[39m"
#define CLI_SGR_BBLACK     "\033[90m"
#define CLI_SGR_BRED       "\033[91m"
#define CLI_SGR_BGREEN     "\033[92m"
#define CLI_SGR_BYELLOW    "\033[93m"
#define CLI_SGR_BBLUE      "\033[94m"
#define CLI_SGR_BMAGENTA   "\033[95m"
#define CLI_SGR_BCYAN      "\033[96m"
#define CLI_SGR_BWHITE     "\033[97m"


namespace system_test {
namespace cli {

    void printModuleTitle(const std::string& module__func__);
    void printTestCaseTitle(const std::string& testCase__func__);
    void printResult(const system_test::Context& ctx);

    /**
     * Prints the instruction and waits until the user pressed enter.
     *
     * @param text The instruction text
     */
    void instruct(const std::string& text);

    /**
     * Prints the yes/no question and waits until the user answered. Updates total and OK counters accordingly.
     *
     * @param [in,out] tcc Current test case counter
     * @param text The question text
     * @retval true If the user answered `y`
     * @retval false If the user answered `n`
     */
    bool check(system_test::TestCaseCounter& tcc, const std::string& text);

} // namespace cli
} // namespace system_test


#endif // IG_SYSTEMTEST_CLI_H
