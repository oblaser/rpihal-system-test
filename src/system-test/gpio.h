/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_SYSTEMTEST_GPIO_H
#define IG_SYSTEMTEST_GPIO_H

#include <cstddef>
#include <cstdint>

#include "system-test/context.h"


namespace system_test {

system_test::TestObejct GPIO_init();

system_test::Module GPIO();

} // namespace system_test


#endif // IG_SYSTEMTEST_GPIO_H
