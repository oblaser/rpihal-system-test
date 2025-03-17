/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <string>

#include "app.h"
#include "middleware/adc.h"
#include "middleware/gpio.h"
#include "middleware/led-bar.h"
#include "project.h"

#include <omw/clock.h>
#include <rpihal/sys.h>


#define LOG_MODULE_LEVEL LOG_LEVEL_INF
#define LOG_MODULE_NAME  APP
#include "middleware/log.h"


using omw::clock::elapsed_ms;
using omw::clock::timepoint_t;


enum
{
    S_init = 0,
    S_idle,
    S_update,
    S_exit,
};

enum
{
    M_potBar = 0,
    M_potValue,
    M_btn1,
    M_temp,

    M__end_,
};



static bool exitSignal = false;
static int state = S_init;
static int mode;
static adc::Result potResult;
static uint8_t btn1Cnt;
static float tempCPU, tempPCB; // degC
static bool showTemp_PCB_nCPU;
static timepoint_t tpUpdate;



static void handleButtons(const timepoint_t& tpNow);
static void setLedBar();
static std::string modeString(int mode);



void app::task()
{
    const timepoint_t tpNow = omw::clock::now();


    handleButtons(tpNow);

    switch (state)
    {
    case S_init:

        mode = 0;
        gpio::led0->clr();
        gpio::led1->clr();

        potResult = 0;
        btn1Cnt = 0;
        tempCPU = 0;
        tempPCB = 0;
        showTemp_PCB_nCPU = true;

        tpUpdate = -omw::clock::second_us; // trigger update immediately

        state = S_idle;

        break;

    case S_idle:
        if (elapsed_ms(tpNow, tpUpdate, 30))
        {
            tpUpdate = tpNow;
            state = S_update;
        }
        break;

    case S_update:

        potResult = adc::readPoti();

        RPIHAL_SYS_getCpuTemp(&tempCPU);

        tempPCB = 3.7499f; // TODO

        setLedBar();

        break;

    case S_exit:
        ledBar::setValue(0);
        exitSignal = true;
        break;

    default:
        if (elapsed_ms(tpNow, tpUpdate, 5 * 1000))
        {
            tpUpdate = tpNow;
            LOG_ERR("invalid state: %i", state);
        }
        break;
    }
}

bool app::exit() { return exitSignal; }



void handleButtons(const timepoint_t& tpNow)
{
    static timepoint_t tpBtn0;

    if (!gpio::btn0->state()) { tpBtn0 = tpNow; }
    if (elapsed_ms(tpNow, tpBtn0, 1000))
    {
        LOG_INF("BTN0 long press");
        state = S_exit;
    }



    if (gpio::btn0->pos()) { LOG_DBG("BTN0 pos"); }
    if (gpio::btn0->neg())
    {
        LOG_DBG("BTN0 neg");

        ++mode;
        if (mode >= M__end_) { mode = 0; }

        static_assert(M__end_ == 4, "LED0,1 can't represent all modes");
        gpio::led0->write((mode & 0x01) != 0);
        gpio::led1->write((mode & 0x02) != 0);

        tpUpdate = -omw::clock::second_us; // trigger update immediately

        LOG_INF("mode: %i %s", mode, modeString(mode).c_str());
    }



    if (gpio::btn1->pos()) { LOG_DBG("BTN1 pos"); }
    if (gpio::btn1->neg())
    {
        LOG_DBG("BTN1 neg");

        if (mode == M_btn1)
        {
            if (potResult.norm() >= 0.75f) { btn1Cnt += 0x10; }
            else if (potResult.norm() >= 0.5f) { ++btn1Cnt; }
            else if (potResult.norm() >= 0.25f) { --btn1Cnt; }
            else { btn1Cnt -= 0x10; }
        }
        else if (mode == M_temp) { showTemp_PCB_nCPU = !showTemp_PCB_nCPU; }
    }
}

void setLedBar()
{
    switch (mode)
    {
    case M_potBar:
        ledBar::setBar((int)(potResult.norm() * 8.0f + 0.5f));
        break;

    case M_potValue:
        ledBar::setValue((uint8_t)(potResult.value() >> 2));
        break;

    case M_btn1:
        ledBar::setValue(btn1Cnt);
        break;

    case M_temp:
    {
        float temp;
        if (showTemp_PCB_nCPU) { temp = tempPCB; }
        else { temp = tempCPU; }

        if ((temp < 0) || (temp >= 127.75f)) { ledBar::setValue(0xFF); }
        else
        {
            temp += 0.25f;                        // round to 0.5
            const int fixed = (int)(temp * 2.0f); // make unsigned fixed point 7.1
            ledBar::setValue((uint8_t)fixed);
        }
    }
    break;

    default:
        LOG_ERR("invalid mode %i", mode);
        break;
    }
}

std::string modeString(int mode)
{
    std::string str;

    switch (mode)
    {
    case M_potBar:
        str = "potentiometer => LED bar";
        break;

    case M_potValue:
        str = "potentiometer => LED bar (as binary value)";
        break;

    case M_btn1:
        str = "BTN1 counter => LED bar";
        break;

    case M_temp:
        str = "PCB/CPU temp => LED bar (fixed point 7.1)";
        break;

    default:
        str = "#" + std::to_string(mode);
        break;
    }

    return str;
}
