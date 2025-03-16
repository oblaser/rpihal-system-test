/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_MIDDLEWARE_ADC_H
#define IG_MIDDLEWARE_ADC_H

#include <cstddef>
#include <cstdint>


namespace adc {

class Result
{
public:
    Result() = delete;

    Result(uint16_t value)
        : m_value(value), m_norm((float)value / 1023.0f)
    {}

    virtual ~Result() {}

    uint16_t value() const { return m_value; }
    float norm() const { return m_norm; } // normalised value in range [0, 1]

private:
    uint16_t m_value;
    float m_norm;
};

int init();
void deinit();

/**
 * @param channel ADC channel [0, 3]
 */
Result read(uint8_t channel);

static inline Result readPoti() { return read(0); }

} // namespace adc


#endif // IG_MIDDLEWARE_ADC_H
