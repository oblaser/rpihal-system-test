/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_SYSTEMTEST_CONTEXT_H
#define IG_SYSTEMTEST_CONTEXT_H

#include <cstddef>
#include <cstdint>


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


#endif // IG_SYSTEMTEST_CONTEXT_H
