/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_SYSTEMTEST_CONTEXT_H
#define IG_SYSTEMTEST_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


#define CTX_CHECK(_to, _bool_expr, _msg) (_to).assert((_bool_expr), (_msg));

#define CTX_REQUIRE(_to, _bool_expr, _msg) \
    {                                      \
        const bool tmp = (_bool_expr);     \
        (_to).assert(tmp, (_msg));         \
        if (!tmp) { return (_to); }        \
    }

#define CTX_ABORT(_to, _msg)         \
    {                                \
        (_to).assert(false, (_msg)); \
        return (_to);                \
    }


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

class Message
{
public:
    Message() = delete;

    Message(const std::string& name, const std::string& message)
        : m_name(name), m_message(message)
    {}

    virtual ~Message() {}

    const std::string& name() const { return m_name; }
    const std::string& message() const { return m_message; }

private:
    std::string m_name;
    std::string m_message;
};



class TestObejct
{
public:
    explicit TestObejct(const std::string& name);

    virtual ~TestObejct() {}

    TestCaseCounter& counter() { return m_counter; }
    const TestCaseCounter& counter() const { return m_counter; }

    const std::string& name() const { return m_name; }
    const std::vector<Message>& messages() const { return m_messages; }


    void setName(const std::string& name);


    void assert(bool expr, const std::string& msg)
    {
        m_counter.add(1, (expr ? 1 : 0));
        if (!expr) { m_messages.push_back(Message(m_name, msg)); }
    }

    const bool allOk() const { return m_counter.allOk(); }


protected:
    TestObejct()
        : m_counter(), m_messages(), m_name()
    {}

    TestCaseCounter m_counter;
    std::vector<Message> m_messages;

private:
    std::string m_name;
};



class Case : public TestObejct
{
public:
    Case() = delete;

    explicit Case(const std::string& name);

    virtual ~Case() {}
};



class Module : public TestObejct
{
public:
    Module() = delete;

    explicit Module(const std::string& name);

    virtual ~Module() {}

    void add(const Case& testCase);
};



class Context
{
public:
    Context()
        : m_counter(), m_messages()
    {}

    virtual ~Context() {}

    void add(const TestObejct& to)
    {
        m_counter.add(to.counter());
        m_messages.insert(m_messages.end(), to.messages().begin(), to.messages().end());
    }

    const TestCaseCounter& counter() const { return m_counter; }
    const std::vector<Message>& messages() const { return m_messages; }

    const bool allOk() const { return m_counter.allOk(); }

private:
    TestCaseCounter m_counter;
    std::vector<Message> m_messages;
};

} // namespace system_test


#endif // IG_SYSTEMTEST_CONTEXT_H
