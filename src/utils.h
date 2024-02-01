#pragma once
#include <Arduino.h>

// Like strncpy, but always null-terminates
extern void strncpynt(char* dest, const char* src, size_t n);