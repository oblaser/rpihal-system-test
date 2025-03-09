/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "context.h"
#include "system-test/cli.h"


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

Case::Case(const std::string& caseName__func__)
    : TestObejct()
{
    cli::printTestCaseTitle(caseName__func__);
}

Module::Module(const std::string& moduleName__func__)
    : TestObejct()
{
    cli::printModuleTitle(moduleName__func__);
}

} // namespace system_test
