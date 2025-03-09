/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

#include "middleware/util.h"
#include "project.h"
#include "rpihal.h"
#include "system-test/context.h"

#include <omw/string.h>
#include <rpihal/rpihal.h>


using std::printf;

namespace {

system_test::TestCaseCounter Model_Detection(int& modelDetectErr)
{
    system_test::TestCaseCounter r(1, 0);

    omw::string testCaseTitle(__func__);
    testCaseTitle.replaceAll('_', ' ');
    printf("\033[36m  --======# \033[96m%s\033[36m #======--\033[39m\n", testCaseTitle.c_str());

    system_test::util::printModelDetectionInfo();

    const auto ans = omw_::cli::choice("\033[97mdoes the detected model match the board which this is running on?\033[39m");

    if (ans == omw_::cli::ChoiceAnswer::A)
    {
        modelDetectErr = 0;
        r.incOk();
    }
    else { modelDetectErr = (-1); }

    return r;
}

} // namespace



int system_test::rpihal(system_test::Context& ctx)
{
    int modelDetectErr = (-1);

    system_test::TestCaseCounter testCaseCnt;

    printf("\033[32m  --======# \033[92m%s\033[32m #======--\033[39m\n", __func__);

    testCaseCnt.add(Model_Detection(modelDetectErr));

    ctx.addCounter(testCaseCnt);

    return modelDetectErr;
}



std::string system_test::util::model_to_string(uint64_t model)
{
    std::string str;

    switch (model)
    {
    case RPIHAL_model_unknown:
        str = "unknown model";
        break;

    case RPIHAL_model_bcm2835:
        str = "BCM2835";
        break;

    case RPIHAL_model_bcm2836:
        str = "BCM2836";
        break;

    case RPIHAL_model_2B:
        str = "2B (pre v1.2)";
        break;

    case RPIHAL_model_bcm2837:
        str = "BCM2837";
        break;

    case RPIHAL_model_2B_v1_2:
        str = "2B v1.2";
        break;

    case RPIHAL_model_3B:
        str = "3B";
        break;

    case RPIHAL_model_cm3:
        str = "CM3";
        break;

    case RPIHAL_model_z2W:
        str = "Zero 2 W";
        break;

    case RPIHAL_model_bcm2837b0:
        str = "BCM2837B0";
        break;

    case RPIHAL_model_3Ap:
        str = "3A+";
        break;

    case RPIHAL_model_3Bp:
        str = "3B+";
        break;

    case RPIHAL_model_cm3p:
        str = "CM3+";
        break;

    case RPIHAL_model_bcm2711:
        str = "BCM2711";
        break;

    case RPIHAL_model_4B:
        str = "4B";
        break;

    case RPIHAL_model_400:
        str = "400";
        break;

    case RPIHAL_model_cm4:
        str = "CM4";
        break;

    case RPIHAL_model_cm4s:
        str = "CM4S";
        break;

    case RPIHAL_model_bcm2712:
        str = "BCM2712";
        break;

    case RPIHAL_model_5:
        str = "5";
        break;

    case RPIHAL_model_500:
        str = "500";
        break;

    case RPIHAL_model_cm5:
        str = "CM5";
        break;

    default:
        printf("\033[91mERR\033[39m model 0x%08llx is unknown to the system-test application (%s:%i)\n", (long long)model, __PRETTY_FUNCTION__,
               (int)(__LINE__));
        str = "ERROR";
        break;
    }

    return str;
}

void system_test::util::printModelDetectionInfo()
{
    const char* dtCompatible = RPIHAL_dt_compatible();
    const char* dtModel = RPIHAL_dt_model();

    if (!dtCompatible) { dtCompatible = "?"; }
    if (!dtModel) { dtModel = "?"; }

    printf("\033[90mdevice tree compatible: %s\033[39m\n", dtCompatible);
    printf("\033[90mdevice tree model:      %s\033[39m\n", dtModel);

    printf("\n");

    const RPIHAL_model_t model = RPIHAL_getModel();
    const std::string modelStr = system_test::util::model_to_string(model);

    printf("detected model:         %s\n", modelStr.c_str());
}
