/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#ifndef IG_MIDDLEWARE_UTIL_H
#define IG_MIDDLEWARE_UTIL_H

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>



#define SIZEOF_ARRAY(_array) (sizeof(_array) / sizeof(_array[0]))



#define UTIL_CLAMP(_v, _lo, _hi) ((_v) < (_lo) ? (_lo) : ((_v) > (_hi) ? (_hi) : (_v)))

// rounds a float or double away from zero, casting to an integer type has to be applied
#define UTIL_ROUND(_v) ((_v) < 0 ? ((_v)-0.5f) : ((_v) + 0.5f))



namespace util {

std::string t_to_iso8601(time_t t);
std::string t_to_iso8601_local(time_t t);
std::string t_to_iso8601_time_local(time_t t);

int sleep(uint32_t t_ms);

} // namespace util



#include <omw/omw.h>
namespace omw_ {

#if OMW_VERSION_ID <= (250201)
namespace cli {

    enum class ChoiceAnswer
    {
        none = 0,
        A,
        B,
        C,
    };

    ChoiceAnswer choice(const std::string& question, const ChoiceAnswer& defaultOption = ChoiceAnswer::none, char optionA = 'y', char optionB = 'n');
    ChoiceAnswer choice(const std::string& question, const ChoiceAnswer& defaultOption, char optionA, char optionB, char optionC);
}
#endif

} // namespace omw_



#if defined(_MSC_VER)
#if (_MSVC_LANG >= 201703L)
#define UTIL_UNUSED [[maybe_unused]]
#else // Cpp std version
#define UTIL_UNUSED
#endif // Cpp std version
#else  // compiler
#define UTIL_UNUSED __attribute__((unused))
#endif // compiler



#endif // IG_MIDDLEWARE_UTIL_H
