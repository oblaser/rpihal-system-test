/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2024 Oliver Blaser
*/

#ifndef IG_PROJECT_H
#define IG_PROJECT_H

#include <omw/version.h>


namespace prj {

const char* const appDirName = "rpihal-system-test"; // use config packName instead

const char* const appName = "System Test for rpihal";
const char* const binName = "rpihal-system-test"; // eq to the linker setting

const omw::Version version(0, 1, 0, "");

const char* const website = "https://github.com/oblaser/rpihal-system-test";

} // namespace prj


#ifdef OMW_DEBUG
#define PRJ_DEBUG (1)
#else
#undef PRJ_DEBUG
#endif



#ifndef RPIHAL_EMU

#ifndef __linux__
#error "not a Linux platform"
#endif

#if (!defined(__arm__) && !defined(__aarch64__))
#error "not an ARM platform" // nothing really ARM speciffic is used, just to detect RasPi
#endif

#endif // RPIHAL_EMU


#endif // IG_PROJECT_H
