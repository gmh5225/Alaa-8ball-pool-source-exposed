#pragma once

#include <basetsd.h>

void fatal(const char* error, const char* _where, SIZE_T errorCode);

int   getRandomInt(const int& min, const int& max);
float getRandomFloat(const float& min, const float& max);