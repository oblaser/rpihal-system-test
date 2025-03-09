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
 * If the detected model is confirmed by the user `modelDetectErr` is set to 0, if the detected model is denied by the
 * user it's set to -1.
 *
 * @param [out] modelDetectErr Status of the model detection
 */
system_test::Module RPIHAL(int& modelDetectErr);



namespace util {

    std::string model_to_string(uint64_t model);
    void printModelDetectionInfo();

} // namespace util

} // namespace system_test


#endif // IG_SYSTEMTEST_RPIHAL_H
