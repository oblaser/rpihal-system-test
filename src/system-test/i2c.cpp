/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "i2c.h"
#include "project.h"
#include "system-test/cli.h"
#include "system-test/context.h"

#include <omw/encoding.h>
#include <omw/string.h>
#include <rpihal/i2c.h>

#include <errno.h>
#include <string.h>
#include <time.h>


using namespace system_test;



static const char* const dev_i2c = "/dev/i2c-1";



static system_test::Case Read_Temp();



system_test::Module system_test::I2C()
{
    Module module(__func__);

    module.add(Read_Temp());

    return module;
}



system_test::Case Read_Temp()
{
    Case tc(__func__);

    int err;
    ssize_t nBytes;

    static constexpr uint8_t addr = 0x48; // TMP1075 address with all Ax pins low
    RPIHAL_I2C_instance_t ___i2c;
    RPIHAL_I2C_instance_t* const i2c = &___i2c;
    uint8_t buffer[3];

    err = RPIHAL_I2C_open(i2c, dev_i2c, addr);
    CTX_REQUIRE(tc, (err == 0), "failed to open I2C device " + std::string(dev_i2c) + " with address 0x" + omw::toHexStr(addr) + " - " + strerror(errno));



    // read device ID

    buffer[0] = 0x0F;
    nBytes = RPIHAL_I2C_write(i2c, buffer, 1);
    CTX_REQUIRE(tc, (nBytes == 1), "failed to set pointer register to read device ID" + std::string(" - ") + strerror(errno));

    nBytes = RPIHAL_I2C_read(i2c, buffer, 2);
    CTX_REQUIRE(tc, (nBytes == 2), "failed to read device ID" + std::string(" - ") + strerror(errno));

    const uint16_t id = omw::bigEndian::decode_ui16(buffer);
    CTX_REQUIRE(tc, ((id & 0xFF00) == 0x7500), "invalid device ID: 0x" + omw::toHexStr(id));



    // trigger one-shot conversion

    buffer[0] = 0x01;
    buffer[1] = 0x80;
    nBytes = RPIHAL_I2C_write(i2c, buffer, 2);
    CTX_CHECK(tc, (nBytes == 2), "failed to write config register" + std::string(" - ") + strerror(errno));

    const struct timespec tsDelay = {
        .tv_sec = 0,
        .tv_nsec = (15 * 1000 * 1000), // conversion time is max 15ms
    };
    err = nanosleep(&tsDelay, NULL);
    if (err) { cli::printWarning(tc, "failed to delay for conversion time" + std::string(" - ") + strerror(errno)); }



    // read temperature

    buffer[0] = 0x00;
    nBytes = RPIHAL_I2C_write(i2c, buffer, 1);
    CTX_REQUIRE(tc, (nBytes == 1), "failed to set pointer register to read temperature" + std::string(" - ") + strerror(errno));

    nBytes = RPIHAL_I2C_read(i2c, buffer, 2);
    CTX_REQUIRE(tc, (nBytes == 2), "failed to read temperature" + std::string(" - ") + strerror(errno));

    const int16_t tempRegister = omw::bigEndian::decode_i16(buffer);
    const float temp = (float)tempRegister * 0.0625f / 16.0f;

    cli::check(tc, "is the read PCB temperaature of " + std::to_string(temp) + OMW_UTF8CP_deg "C plausible?");



    err = RPIHAL_I2C_close(i2c);
    CTX_CHECK(tc, (err == 0), "failed to close I2C instance" + std::string(" - ") + strerror(errno));

    return tc;
}
