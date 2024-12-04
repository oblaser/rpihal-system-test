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



namespace system_test {

class TestCaseCounter
{
public:
    TestCaseCounter()
        : m_total(0), m_ok(0)
    {}

    TestCaseCounter(size_t total, size_t ok)
        : m_total(total), m_ok(ok)
    {}

    virtual ~TestCaseCounter() {}

    void add(size_t total, size_t ok);
    void add(const TestCaseCounter& counter);

    void addOk(size_t value) { m_ok += value; }
    void incOk() { ++m_ok; }

    size_t total() const { return m_total; }
    size_t ok() const { return m_ok; }

private:
    size_t m_total;
    size_t m_ok;
};

class Context
{
public:
    Context()
        : m_counter()
    {}

    virtual ~Context() {}

    void addCounter(const TestCaseCounter& counter) { m_counter.add(counter); }

    const TestCaseCounter& counter() const { return m_counter; }

private:
    TestCaseCounter m_counter;
};

} // namespace system_test



#include <omw/omw.h>
namespace omw_ {

#if OMW_VERSION_ID <= OMW_VERSION_ID_0_2_1_BETA
namespace cli {

    enum class ChoiceAnswer
    {
        none = 0,
        A,
        B,
        C,
    };

    ChoiceAnswer choice(const std::string& question, const ChoiceAnswer& defaultOption = ChoiceAnswer::none, char optionA = 'y', char optionB = 'n');
    ChoiceAnswer choice(const std::string& question, const ChoiceAnswer& defaultOption, char optionA, char optionB, char option);
}
#endif

}



#endif // IG_MIDDLEWARE_UTIL_H
