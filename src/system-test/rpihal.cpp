/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>

#include "middleware/util.h"
#include "project.h"
#include "system-test/rpihal.h"

#include <omw/string.h>
#include <rpihal/rpihal.h>


namespace {

system_test::TestCaseCounter Model_Detection()
{
    system_test::TestCaseCounter r(1, 0);

    {
        omw::string testCaseTitle(__func__);
        testCaseTitle.replaceAll('_', ' ');
        printf("\033[36m  --======# \033[96m%s\033[36m #======--\033[39m\n", testCaseTitle.c_str());
    }

    {
        const char* dtCompatible = RPIHAL_dt_compatible();
        const char* dtModel = RPIHAL_dt_model();
        if (!dtCompatible) { dtCompatible = "?"; }
        if (!dtModel) { dtModel = "?"; }
        printf("\033[90mdevice tree compatible: %s\033[39m\n", dtCompatible);
        printf("\033[90mdevice tree model:      %s\033[39m\n", dtModel);
    }

    printf("\n");

    {
        const RPIHAL_model_t model = RPIHAL_getModel();
        std::string modelStr = "?";

        switch (model)
        {
        case RPIHAL_model_unknown:
            modelStr = "unknown model";
            break;

        case RPIHAL_model_bcm2835:
            modelStr = "BCM2835";
            break;

        case RPIHAL_model_bcm2836:
            modelStr = "BCM2836";
            break;

        case RPIHAL_model_2B:
            modelStr = "2B (pre v1.2)";
            break;

        case RPIHAL_model_bcm2837:
            modelStr = "BCM2837";
            break;

        case RPIHAL_model_2B_v1_2:
            modelStr = "2B v1.2";
            break;

        case RPIHAL_model_3B:
            modelStr = "3B";
            break;

        case RPIHAL_model_cm3:
            modelStr = "CM3";
            break;

        case RPIHAL_model_z2W:
            modelStr = "Zero 2 W";
            break;

        case RPIHAL_model_bcm2837b0:
            modelStr = "BCM2837B0";
            break;

        case RPIHAL_model_3Ap:
            modelStr = "3A+";
            break;

        case RPIHAL_model_3Bp:
            modelStr = "3B+";
            break;

        case RPIHAL_model_cm3p:
            modelStr = "CM3+";
            break;

        case RPIHAL_model_bcm2711:
            modelStr = "BCM2711";
            break;

        case RPIHAL_model_4B:
            modelStr = "4B";
            break;

        case RPIHAL_model_400:
            modelStr = "400";
            break;

        case RPIHAL_model_cm4:
            modelStr = "CM4";
            break;

        case RPIHAL_model_bcm2712:
            modelStr = "BCM2712";
            break;

        default:
            printf("\033[91mERR\033[39m model 0x%08llx is unknown in system-test (%s:%i)\n", (long long)model, __PRETTY_FUNCTION__, (int)(__LINE__));
            modelStr = "ERROR";
            break;
        }

        printf("detected model:         %s\n", modelStr.c_str());
    }

    const auto ans = omw_::cli::choice("\033[97mdoes the detected model match the board which this is running on?\033[39m");

    if (ans == omw_::cli::ChoiceAnswer::A) { r.incOk(); }

    return r;
}

} // namespace



void system_test::rpihal(system_test::Context& ctx)
{
    system_test::TestCaseCounter testCaseCnt;

    printf("\033[32m  --======# \033[92m%s\033[32m #======--\033[39m\n", __func__);

    testCaseCnt.add(Model_Detection());

    ctx.addCounter(testCaseCnt);
}
