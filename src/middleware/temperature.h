/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_MIDDLEWARE_TEMPERATURE_H
#define IG_MIDDLEWARE_TEMPERATURE_H

#include <cstddef>
#include <cstdint>


namespace temp {

int init();
void deinit();

float get();

}


#endif // IG_MIDDLEWARE_TEMPERATURE_H
