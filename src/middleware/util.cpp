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





#ifdef OMW_PLAT_WIN
#else // OMW_PLAT_WIN
#include <time.h>
#endif // OMW_PLAT_WIN

omw_::clock::timepoint_t omw_::clock::get()
{
    timepoint_t r = 0;

#if defined(OMW_PLAT_POSIX)

#ifdef OMW_PLAT_LINUX
    constexpr clockid_t clockId = CLOCK_BOOTTIME;
#else
    constexpr clockid_t clockId = CLOCK_MONOTONIC;
#endif

    struct timespec tp;
    if (clock_gettime(clockId, &tp) == 0) { r = fromTimespec(tp.tv_sec, tp.tv_nsec); }
    else { r = 0; }

#elif defined(OMW_PLAT_WIN)
#error "not yet implemented for MSW"
#else // OMW_PLAT_
#error "platform not supported"
#endif // OMW_PLAT_

    return r;
}
