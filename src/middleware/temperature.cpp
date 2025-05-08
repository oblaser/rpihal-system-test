/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "project.h"
#include "temperature.h"

#include <omw/encoding.h>

#include <rpihal/i2c.h>

#include <sys/types.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  TEMP
#include "middleware/log.h"


#ifdef _MSC_VER // clang-format off
#pragma warning(disable: 4996)
#endif // clang-format on



static RPIHAL_I2C_instance_t ___i2c;
static RPIHAL_I2C_instance_t* const i2c = &___i2c;
static uint8_t buffer[3];



static int setPointerRegister(uint8_t regAddr);

#ifdef RPIHAL_EMU
extern "C" ssize_t temp_i2c_emu_read_callback(uint8_t* buffer, size_t count);
extern "C" ssize_t temp_i2c_emu_write_callback(const uint8_t* data, size_t count);
#endif



int temp::init()
{
    int err;
    ssize_t nBytes;



    err = RPIHAL_I2C_open(i2c, "/dev/i2c-1", 0x48);
    if (err)
    {
        LOG_ERR("failed to open I2C, err: %i, errno: %i %s", err, errno, std::strerror(errno));
        return -(__LINE__);
    }

#ifdef RPIHAL_EMU
    i2c->read_cb = temp_i2c_emu_read_callback;
    i2c->write_cb = temp_i2c_emu_write_callback;
#endif



// read all register values
#if PRJ_DEBUG && 0
    {
        constexpr size_t nRegisters = 16;

        for (size_t i = 0; i < nRegisters; ++i)
        {
            if (setPointerRegister((uint8_t)i) == 0)
            {
                uint8_t regBuffer[2] = { 0, 0 };

                const size_t count = 2;

                nBytes = RPIHAL_I2C_read(i2c, regBuffer, count);
                if (nBytes != (ssize_t)count) { LOG_ERR("failed to read registers, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno)); }
                else
                {
                    int value = (int)regBuffer[0];
                    value <<= 8;
                    value |= (int)regBuffer[1];

                    LOG_INF("TMP1075 reg 0x%02zX: 0x%04x", i, value);
                }
            }
        }
    }
#endif



    // read device ID

    if (setPointerRegister(0x0F) != 0) { return -(__LINE__); }
    nBytes = RPIHAL_I2C_read(i2c, buffer, 2);
    if (nBytes != 2)
    {
        LOG_ERR("failed to read device ID, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno));
        return -(__LINE__);
    }

    const uint16_t id = omw::bigEndian::decode_ui16(buffer);

    if (((id & 0xFF00) != 0x7500))
    {
        LOG_ERR("invalid device ID: 0x%02x", (int)id);
        return -(__LINE__);
    }



    // config 220ms continuous conversion mode

    buffer[0] = 0x01;
    buffer[1] = 0x60;
    nBytes = RPIHAL_I2C_write(i2c, buffer, 2);
    if (nBytes != 2)
    {
        LOG_ERR("failed to write config register, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno));
        return -(__LINE__);
    }



    return 0;
}

void temp::deinit()
{
    int err;

    err = RPIHAL_I2C_close(i2c);
    if (err) { LOG_ERR("failed to close I2C, err: %i, errno: %i %s", err, errno, std::strerror(errno)); }
}

float temp::get()
{
    ssize_t nBytes;

    if (setPointerRegister(0x00) != 0) { return -(__LINE__); }

    nBytes = RPIHAL_I2C_read(i2c, buffer, 2);
    if (nBytes != 2)
    {
        LOG_ERR("failed to read temperature, err: %lli, errno: %i %s", (long long)nBytes, errno, std::strerror(errno));
        return -9999;
    }

    const int16_t tempRegister = omw::bigEndian::decode_i16(buffer);
    const float temp = (float)tempRegister * 0.0625f / 16.0f;

    return temp;
}



int setPointerRegister(uint8_t regAddr)
{
    const ssize_t res = RPIHAL_I2C_write(i2c, &regAddr, 1);

    if (res != 1)
    {
        LOG_ERR("failed to set pointer register to 0x%02x, err: %lli, errno: %i %s", (int)regAddr, (long long)res, errno, std::strerror(errno));
        return -(__LINE__);
    }

    return 0;
}



#ifdef RPIHAL_EMU

class EmulatedTMP1075
{
public:
    EmulatedTMP1075()
        : m_registers{}, m_regPointer(0)
    {
        m_resetRegisters();
    }

    explicit EmulatedTMP1075(float temp)
        : m_registers{}, m_regPointer(0)
    {
        m_resetRegisters();
        setTemp(temp);
    }

    virtual ~EmulatedTMP1075() {}

    void setTemp(float temp)
    {
        const float rh = (temp < 0 ? -0.5f : 0.5f);           // rounding helper
        const uint16_t value = (uint16_t)(temp * 16.0f + rh); // value as 8.4 fixed point decimal number
        m_registers[0x00] = (value << 4);

#if (LOG_MODULE_LEVEL >= LOG_LEVEL_DBG) && 0
        printf("%s %f, fixed point: %i 0x%04x, reg: 0x%04x %f\n",
#ifdef _MSC_VER
               __FUNCSIG__,
#else
               __PRETTY_FUNCTION__,
#endif
               (double)temp, (int)value, (int)value, (int)m_registers[0x00], ((int16_t)m_registers[0x00] * 0.0625f / 16.0f));
#endif // log level
    }

    ssize_t i2c_read(uint8_t* buffer, size_t count)
    {
        const ssize_t r = (ssize_t)count;

        if (count > (2 * m_nRegisters)) { return -1; }

        uint8_t regOffs = 0;
        uint8_t byteOffs = 0;

        while (count)
        {
            const uint8_t regAddr = m_regPointer + regOffs;

            if (!m_checkRegAccess(regAddr, false)) { return -1; } // NAK register access denied

            if (regOffs == 0) // TMP1075 does only return the one register specified
            {
                if (byteOffs == 0) { *buffer = (uint8_t)(m_registers[regAddr] >> 8); }
                else { *buffer = (uint8_t)(m_registers[regAddr]); }
            }
            else { *buffer = 0xFF; }

            ++byteOffs;
            if (byteOffs >= 2)
            {
                byteOffs = 0;
                ++regOffs;
            }

            ++buffer;
            --count;
        }

        return r;
    }

    ssize_t i2c_write(const uint8_t* data, size_t count)
    {
        const ssize_t r = (ssize_t)count;

        if (count > (1 + 2 * m_nRegisters)) { return -1; }

        if (count != 0)
        {
            if (data[0] & 0xF0) { return -1; } // NAK on invalid pointer register value

            m_regPointer = data[0];

            ++data;
            --count;
        }

        uint8_t regOffs = 0;
        uint8_t byteOffs = 0;

        while (count)
        {
            const uint8_t regAddr = m_regPointer + regOffs;

            if (!m_checkRegAccess(regAddr, false)) { return -1; } // NAK register access denied

            if (regOffs == 0) // TMP1075 does only allow writing one register at a time
            {
                if (byteOffs == 0)
                {
                    m_registers[regAddr] &= ~0xFF00;
                    m_registers[regAddr] |= ((uint16_t)(*data) << 8);
                }
                else
                {
                    m_registers[regAddr] &= ~0x00FF;
                    m_registers[regAddr] |= (uint16_t)(*data);
                }
            }
            else { return -1; } // NAK

            ++byteOffs;
            if (byteOffs >= 2)
            {
                byteOffs = 0;
                ++regOffs;
            }

            --count;
        }

        return r;
    }

private:
    static constexpr uint8_t m_nRegisters = 16;
    uint16_t m_registers[m_nRegisters];
    uint8_t m_regPointer;

    void m_resetRegisters()
    {
        for (size_t i = 0; i < (size_t)m_nRegisters; ++i) { m_registers[i] = 0; }

        m_registers[0x00] = 0x0000;
        m_registers[0x01] = 0x00FF;
        m_registers[0x02] = 0x4B00;
        m_registers[0x03] = 0x5000;
        m_registers[0x0F] = 0x7500;
    }

    bool m_checkRegAccess(uint8_t regAddr, bool write_nRead)
    {
        bool r = false;

        if (write_nRead)
        {
            if ((regAddr == 0x01) || (regAddr == 0x02) || (regAddr == 0x03)) { r = true; }
        }
        else
        {
            if ((regAddr >= 0x00) && (regAddr <= 0x0F)) { r = true; }
        }

        return r;
    }
};

#if PRJ_DEBUG
static EmulatedTMP1075 ___dbg0 = EmulatedTMP1075(0.0f);
static EmulatedTMP1075 ___dbg1 = EmulatedTMP1075(-25.0f);
static EmulatedTMP1075 ___dbg2 = EmulatedTMP1075(-0.25f);
static EmulatedTMP1075 ___dbg3 = EmulatedTMP1075(80.0f);
static EmulatedTMP1075 ___dbg4 = EmulatedTMP1075(33.75f);
static EmulatedTMP1075 ___dbg5 = EmulatedTMP1075(-22.35f);
#endif

static EmulatedTMP1075 TMP1075_48 = EmulatedTMP1075(22.35f);

extern "C" ssize_t temp_i2c_emu_read_callback(uint8_t* buffer, size_t count) { return TMP1075_48.i2c_read(buffer, count); }
extern "C" ssize_t temp_i2c_emu_write_callback(const uint8_t* data, size_t count) { return TMP1075_48.i2c_write(data, count); }

#endif // RPIHAL_EMU
