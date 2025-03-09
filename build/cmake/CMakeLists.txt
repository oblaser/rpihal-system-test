
# author        Oliver Blaser
# copyright     MIT - Copyright (c) 2024 Oliver Blaser

cmake_minimum_required(VERSION 3.13)

project(rpihal-system-test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# detect if target platform is RasPi (actually any ARM platform, may be improved)
set(PLAT_IS_RASPI false)
#message(${CMAKE_SYSTEM_PROCESSOR})
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(armv[6-8]([A-Z]|[a-z])?)" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(PLAT_IS_RASPI true)
endif()



#
# SDK
#

# omw
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../sdk/omw/build/cmake/libomw.a/ ${CMAKE_CURRENT_LIST_DIR}/../../sdk/omw/build/cmake/libomw.a/)
include_directories(../../sdk/omw/include)

# rpihal
include_directories(../../sdk/rpihal/include)
if(_DEBUG)
    add_definitions(-DRPIHAL_CONFIG_LOG_LEVEL=2)
else()
    add_definitions(-DRPIHAL_CONFIG_LOG_LEVEL=2)
endif()
if(NOT PLAT_IS_RASPI)
    add_definitions(-DRPIHAL_EMU)
    set(RPIHAL_CMAKE_CONFIG_EMU true)
endif()
add_subdirectory(../../sdk/rpihal/build/librpihal.a/ ../../sdk/rpihal/build/librpihal.a/)


#
# the application
#

set(BINNAME rpihal-system-test)

if(_DEBUG)
    add_definitions(-D_DEBUG)
    add_definitions(-DCONFIG_LOG_LEVEL=4)
else()
    add_definitions(-DCONFIG_LOG_LEVEL=2)
endif()

include_directories(../../src/)

set(SOURCES
../../src/middleware/gpio.cpp
../../src/middleware/util.cpp
../../src/system-test/cli.cpp
../../src/system-test/context.cpp
../../src/system-test/gpio.cpp
../../src/system-test/rpihal.cpp
../../src/main.cpp
)



add_executable(${BINNAME} ${SOURCES})
target_link_libraries(${BINNAME} omw rpihal)
target_compile_options(${BINNAME} PRIVATE -Wno-psabi -Wall -Werror=return-type -Werror=switch -Werror=reorder -Werror=format)



if(PLAT_IS_RASPI)
    message(STATUS "we are on the Pi :)")
else()
    message("")
    message("########################################")
    message("########################################")
    message("###                                  ###")
    message("###           not on a Pi            ###")
    message("###                                  ###")
    message("########################################")
    message("########################################")
    message("")
endif()
