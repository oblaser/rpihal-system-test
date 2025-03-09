/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include "util.h"

#include <omw/defs.h>

#ifdef OMW_PLAT_WIN
#include <Windows.h>
#else // OMW_PLAT_WIN
#include <unistd.h>
#endif // OMW_PLAT_WIN


namespace {}



std::string util::t_to_iso8601(time_t t)
{
    std::string r;

    constexpr size_t bufferSize = 100;
    char buffer[bufferSize];

    const struct std::tm* tm = std::localtime(&t);

    if (tm && (std::strftime(buffer, bufferSize, "%FT%T%z", tm) > 0)) { r = std::string(buffer); }
    else { r = '[' + std::to_string(t) + ']'; }

    return r;
}

std::string util::t_to_iso8601_local(time_t t)
{
    std::string r;

    constexpr size_t bufferSize = 100;
    char buffer[bufferSize];

    const struct std::tm* tm = std::localtime(&t);

    if (tm && (std::strftime(buffer, bufferSize, "%FT%T", tm) > 0)) { r = std::string(buffer); }
    else { r = '[' + std::to_string(t) + ']'; }

    return r;
}

std::string util::t_to_iso8601_time_local(time_t t)
{
    std::string r;

    constexpr size_t bufferSize = 100;
    char buffer[bufferSize];

    const struct std::tm* tm = std::localtime(&t);

    if (tm && (std::strftime(buffer, bufferSize, "%T", tm) > 0)) { r = std::string(buffer); }
    else { r = '[' + std::to_string(t) + ']'; }

    return r;
}

int util::sleep(unsigned t_ms)
{
#ifdef OMW_PLAT_WIN
    Sleep(t_ms);
    return 0;
#else  // OMW_PLAT_WIN
    return usleep(t_ms * 1000);
#endif // OMW_PLAT_WIN
}



//======================================================================================================================
// omw_



#include <iostream>
#include <string>
omw_::cli::ChoiceAnswer omw_::cli::choice(const std::string& question, const ChoiceAnswer& defaultOption, char optionA, char optionB)
{
    ChoiceAnswer r = ChoiceAnswer::none;
    const omw::string a(1, optionA);
    const omw::string b(1, optionB);
    omw::string data;

    do {
        std::cout << question << " [" << (defaultOption == ChoiceAnswer::A ? a.toUpper_ascii() : a) << "/"
                  << (defaultOption == ChoiceAnswer::B ? b.toUpper_ascii() : b) << "] ";
        std::getline(std::cin, data);

        if (data.toLower_ascii() == a) { r = ChoiceAnswer::A; }
        else if (data.toLower_ascii() == b) { r = ChoiceAnswer::B; }
        else if (data.length() == 0) { r = defaultOption; }
        else { r = ChoiceAnswer::none; }
    }
    while ((r != ChoiceAnswer::A) && (r != ChoiceAnswer::B));

    return r;
}
