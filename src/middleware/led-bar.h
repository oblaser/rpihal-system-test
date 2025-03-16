/*
author          Oliver Blaser
copyright       MIT - Copyright (c) 2025 Oliver Blaser
*/

#ifndef IG_MIDDLEWARE_LEDBAR_H
#define IG_MIDDLEWARE_LEDBAR_H

#include <cstddef>
#include <cstdint>


namespace ledBar {

int init();
void deinit();

/**
 * @param value Bar value in range [0, 8]
 */
void setBar(int value);

void setValue(uint8_t value);

}


#endif // IG_MIDDLEWARE_LEDBAR_H
