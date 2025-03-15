/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "context.h"
#include "system-test/cli.h"

#include <omw/string.h>


namespace system_test {

void TestCaseCounter::add(size_t total, size_t ok)
{
    m_total += total;
    m_ok += ok;
}

void TestCaseCounter::add(const TestCaseCounter& counter)
{
    m_total += counter.total();
    m_ok += counter.ok();
}

TestObejct::TestObejct(const std::string& name)
    : m_counter(), m_messages(), m_name(name)
{
    // this->setName(name); not here

    cli::printUnassocTitle(this->name());
}

void TestObejct::setName(const std::string& name)
{
    m_name = name;
    omw::replaceAll(m_name, '_', ' ');
}

Case::Case(const std::string& name)
    : TestObejct()
{
    this->setName(name);

    cli::printTestCaseTitle(this->name());
}

Module::Module(const std::string& name)
    : TestObejct()
{
    this->setName(name);

    cli::printModuleTitle(this->name());
}

void Module::add(const Case& testCase)
{
    m_counter.add(testCase.counter());

    for (size_t i = 0; i < testCase.messages().size(); ++i)
    {
        constexpr size_t moduleNameWidth = 10;
        const size_t nFill = moduleNameWidth - this->name().length();

        const Message msg(this->name() + std::string(nFill + 1, ' ') + testCase.messages()[i].name(), testCase.messages()[i].message());

        m_messages.push_back(msg);
    }
}

} // namespace system_test
