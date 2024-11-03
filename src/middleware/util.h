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

int sleep(unsigned t_ms);

} // namespace util



namespace omw_::clock { // monotonic clock/counter

/**
 * @brief Counter value with 1us resolution.
 */
using timepoint_t = int64_t;
// int64 max = 9'223'372'036'854'775'807
//                    10'000'000'000'000 ms = approx 316 years
//                10'000'000'000'000'000 us = approx 316 years (no sowtware runns over 300 years without restart ;)



static constexpr timepoint_t second_s = 1;
static constexpr timepoint_t minute_s = 60 * second_s;
static constexpr timepoint_t hour_s = 60 * minute_s;
static constexpr timepoint_t day_s = 24 * hour_s;

static constexpr timepoint_t second_ms = second_s * 1000;
static constexpr timepoint_t minute_ms = minute_s * 1000;
static constexpr timepoint_t hour_ms = hour_s * 1000;
static constexpr timepoint_t day_ms = day_s * 1000;

static constexpr timepoint_t second_us = second_ms * 1000;
static constexpr timepoint_t minute_us = minute_ms * 1000;
static constexpr timepoint_t hour_us = hour_ms * 1000;
static constexpr timepoint_t day_us = day_ms * 1000;



/**
 * @brief Returns the current counter value.
 *
 * - Posix: `CLOCK_MONOTONIC`
 * - Linux: `CLOCK_BOOTTIME`
 * - Windows: `PerformanceCounter`
 */
timepoint_t get();

static inline bool elapsed_us(timepoint_t now_us, timepoint_t last_us, timepoint_t interval_us) { return ((now_us - last_us) >= interval_us); }
static inline bool elapsed_ms(timepoint_t now_us, timepoint_t last_us, uint32_t interval_ms)
{
    return elapsed_us(now_us, last_us, ((timepoint_t)interval_ms * (timepoint_t)(1000)));
}

static inline timepoint_t fromTimespec(time_t tv_sec, long tv_nsec) // see Linux struct timespec
{
    // TODO omw_ sign check
    timepoint_t us = (int64_t)tv_sec * 1000000ll;
    us += (int64_t)tv_nsec / 1000ll;
    return us;
}

} // namespace omw_::clock


#endif // IG_MIDDLEWARE_UTIL_H
