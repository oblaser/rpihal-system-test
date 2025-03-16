/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

#include "project.h"
#include "sys.h"
#include "system-test/cli.h"
#include "system-test/context.h"

#include <omw/int.h>
#include <omw/string.h>
#include <rpihal/sys.h>


using namespace system_test;



static system_test::Case Read_CPU_Temp();
static system_test::Case Machine_ID();

static int readFile(const std::string& filename, std::string& buffer);
static int readFileFirstLine(const std::string& filename, std::string& buffer);



system_test::Module system_test::SYS()
{
    Module module(__func__);

    module.add(Read_CPU_Temp());
    module.add(Machine_ID());

    return module;
}



system_test::Case Read_CPU_Temp()
{
    Case tc(__func__);

    int err;



    float tempRpihal;
    err = RPIHAL_SYS_getCpuTemp(&tempRpihal);
    CTX_REQUIRE(tc, !err, "RPIHAL_SYS_getCpuTemp() failed");



    const std::string filename = "/sys/class/thermal/thermal_zone0/temp";
    std::string tempStr;
    err = readFileFirstLine(filename, tempStr);
    CTX_REQUIRE(tc, !err, "failed to read file \"" + filename + "\"");
    CTX_REQUIRE(tc, omw::isInteger(tempStr), "read file text is not an integer");



    const float tempTest = (float)std::stoi(tempStr) / 1e3f;
    const float delta = tempTest - tempRpihal;
    constexpr float tolerance = 3.0f;
    CTX_CHECK(tc, ((delta <= tolerance) && (delta >= -tolerance)),
              "invalid CPU temperature, RPIHAL: " + std::to_string(tempRpihal) + OMW_UTF8CP_deg "C, test: " + std::to_string(tempTest) + OMW_UTF8CP_deg "C");



    return tc;
}

system_test::Case Machine_ID()
{
    Case tc(__func__);

    int err;



    const RPIHAL_uint128_t machineIdRpihal = RPIHAL_SYS_getMachineId();
    const omw::uint128_t machineId(machineIdRpihal.hi, machineIdRpihal.lo);



    const std::string filename = "/etc/machine-id";
    std::string machineIdStr;
    err = readFileFirstLine(filename, machineIdStr);
    CTX_REQUIRE(tc, !err, "failed to read file \"" + filename + "\"");
    CTX_REQUIRE(tc, omw::isHex(machineIdStr), "read file text is not hex");
    CTX_REQUIRE(tc, (machineIdStr.length() <= 32), "read file text would overflow uint128");



    if (machineIdStr.length() & 1) { machineIdStr.insert(machineIdStr.begin(), '0'); }

    CTX_CHECK(tc, (machineId == omw::hexstoui128(machineIdStr)), "invalid machine ID, RPIHAL: " + omw::toHexStr(machineId) + ", test: " + machineIdStr);

    return tc;
}



int readFile(const std::string& filename, std::string& buffer)
{
    constexpr size_t blockSize = 1024;
    char block[blockSize + 1];

    std::ifstream ifs;
    ifs.open(filename, std::ios::in | std::ios::binary);

    if (!ifs.good()) { return -(__LINE__); }

    buffer.clear();

    while (ifs.good())
    {
        ifs.read(block, blockSize);
        const size_t nBytes = (size_t)ifs.gcount();

        if (ifs.eof()) { block[nBytes] = 0; }
        else { block[blockSize] = 0; }

        buffer.append(block);
    }

    ifs.close();

    return 0;
}

int readFileFirstLine(const std::string& filename, std::string& buffer)
{
    const int err = readFile(filename, buffer);

    if (!err)
    {
        buffer = omw::split(buffer, '\r')[0];
        buffer = omw::split(buffer, '\n')[0];
    }

    return err;
}
