/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_SYSTEMTEST_RPIHAL_H
#define IG_SYSTEMTEST_RPIHAL_H

#include <cstddef>
#include <cstdint>
#include <string>

#include "system-test/context.h"


namespace system_test {

/**
 * @brief Runs the system tests for the RPIHAL module.
 *
 * @param [in,out] ctx System test context
 * @return Status of the model detection
 * @retval 0 if the detected model is confirmed by the user
 * @retval -1 if the detected model is denied by the user
 */
int rpihal(system_test::Context& ctx);



namespace util {

    std::string model_to_string(uint64_t model);
    void printModelDetectionInfo();

} // namespace util

} // namespace system_test


#endif // IG_SYSTEMTEST_RPIHAL_H
