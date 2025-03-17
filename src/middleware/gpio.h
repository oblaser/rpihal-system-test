/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#ifndef IG_MIDDLEWARE_GPIO_H
#define IG_MIDDLEWARE_GPIO_H

#include <cstddef>
#include <cstdint>

#include "gpio-pins.h"
#include "project.h"

#include <rpihal/gpio.h>


namespace gpio {

/**
 * @brief Initialises the GPIOs needed by this project.
 *
 * @return 0 on success
 */
int init();

/**
 * @brief Deinitialises only the GPIOs initialised by this project
 *
 * @return 0 on success
 */
void deinit();

void task();



class EdgeDetect // not really GPIO specific, could be moved to another header
{
public:
    EdgeDetect()
        : m_state(), m_old(), m_pos(), m_neg()
    {}

    EdgeDetect(bool state, bool oldState)
        : m_state(state), m_old(oldState), m_pos(), m_neg()
    {}

    virtual ~EdgeDetect() {}

    bool state() const { return m_state; }
    bool pos() const { return m_pos; }
    bool neg() const { return m_neg; }

    void handler(bool state)
    {
        m_state = state;
        m_pos = (!m_old && m_state);
        m_neg = (m_old && !m_state);
        m_old = m_state;
    }

private:
    bool m_state, m_old, m_pos, m_neg;
};

class Button : public EdgeDetect
{
public:
    Button() = delete;

    explicit Button(int pin)
        : EdgeDetect(), m_pin(pin)
    {
        handler();
        handler();
    }

    virtual ~Button() {}

    virtual void handler() { EdgeDetect::handler(RPIHAL_GPIO_readPin(m_pin) > 0); }

protected:
    int m_pin;
};

class ButtonInverted : public Button
{
public:
    ButtonInverted() = delete;

    explicit ButtonInverted(int pin)
        : Button(pin)
    {
        handler();
        handler();
    }

    virtual ~ButtonInverted() {}

    virtual void handler() { EdgeDetect::handler(RPIHAL_GPIO_readPin(m_pin) == 0); }
};

class Input : private EdgeDetect
{
public:
    Input() = delete;

    Input(int pin, bool state)
        : EdgeDetect(state, state), m_pin(pin)
    {}

    virtual ~Input() {}

    bool state() const { return EdgeDetect::state(); }
    bool pos() const { return EdgeDetect::pos(); }
    bool neg() const { return EdgeDetect::neg(); }

    virtual void handler() = 0;

protected:
    int m_pin;

    void m_handler(bool state) { EdgeDetect::handler(state); }

private:
    Input(const Input& other) = delete;
    Input(const Input&& other) = delete;
    Input& operator=(const Input& other);
};

class InputActiveHigh : public Input
{
public:
    InputActiveHigh() = delete;

    explicit InputActiveHigh(int pin)
        : Input(pin, 0)
    {}

    virtual ~InputActiveHigh() {}

    virtual void handler() { m_handler(RPIHAL_GPIO_readPin(m_pin) > 0); }
};

class InputActiveLow : public Input
{
public:
    InputActiveLow() = delete;

    explicit InputActiveLow(int pin)
        : Input(pin, 1)
    {}

    virtual ~InputActiveLow() {}

    virtual void handler() { m_handler(RPIHAL_GPIO_readPin(m_pin) == 0); }
};

class Output
{
public:
    Output() = delete;

    explicit Output(int pin)
        : m_pin(pin)
    {}

    // Output(int pin, bool state)          don't do that, will call RPIHAL before RPIHAL init
    //     : m_pin(pin)
    // {
    //     this->write(state);
    // }

    virtual ~Output() {}

    virtual bool read() const = 0;
    virtual void write(bool state) = 0;
    virtual void set() { this->write(1); }
    virtual void clr() { this->write(0); }
    virtual void toggle() { RPIHAL_GPIO_togglePin(m_pin); }

protected:
    int m_pin;

private:
    Output(const Output& other) = delete;
    Output(const Output&& other) = delete;
    Output& operator=(const Output& other);
};

class OutputActiveHigh : public Output
{
public:
    OutputActiveHigh() = delete;

    explicit OutputActiveHigh(int pin)
        : Output(pin)
    {}

    virtual ~OutputActiveHigh() {}

    virtual bool read() const { return (RPIHAL_GPIO_readPin(m_pin) > 0); }
    virtual void write(bool state) { RPIHAL_GPIO_writePin(m_pin, state); }
};

class OutputActiveLow : public Output
{
public:
    OutputActiveLow() = delete;

    explicit OutputActiveLow(int pin)
        : Output(pin)
    {}

    virtual ~OutputActiveLow() {}

    virtual bool read() const { return (RPIHAL_GPIO_readPin(m_pin) == 0); }
    virtual void write(bool state) { RPIHAL_GPIO_writePin(m_pin, (state ? 0 : 1)); }
};

extern Input* const btn0;
extern Input* const btn1;

extern Output* const led0;
extern Output* const led1;

} // namespace gpio


#endif // IG_MIDDLEWARE_GPIO_H
