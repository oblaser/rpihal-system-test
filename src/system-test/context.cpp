/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "context.h"


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

} // namespace system_test
