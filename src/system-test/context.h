/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_SYSTEMTEST_CONTEXT_H
#define IG_SYSTEMTEST_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <string>


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

    const bool allOk() const { return (m_total == m_ok); }

private:
    size_t m_total;
    size_t m_ok;
};



class TestObejct
{
public:
    explicit TestObejct(const std::string& name);

    virtual ~TestObejct() {}

    TestCaseCounter& counter() { return m_counter; }
    const TestCaseCounter& counter() const { return m_counter; }

    void assert(bool value) { m_counter.add(1, (value ? 1 : 0)); }

    const bool allOk() const { return m_counter.allOk(); }

protected:
    TestObejct()
        : m_counter()
    {}

    TestCaseCounter m_counter;
};



class Case : public TestObejct
{
public:
    Case() = delete;

    explicit Case(const std::string& caseName__func__);

    virtual ~Case() {}
};



class Module : public TestObejct
{
public:
    Module() = delete;

    explicit Module(const std::string& moduleName__func__);

    virtual ~Module() {}

    void add(const Case& testCase) { m_counter.add(testCase.counter()); }
};



class Context
{
public:
    Context()
        : m_counter()
    {}

    virtual ~Context() {}

    void add(const TestObejct& to) { m_counter.add(to.counter()); }

    const TestCaseCounter& counter() const { return m_counter; }

    const bool allOk() const { return m_counter.allOk(); }

private:
    TestCaseCounter m_counter;
};

} // namespace system_test


#endif // IG_SYSTEMTEST_CONTEXT_H
