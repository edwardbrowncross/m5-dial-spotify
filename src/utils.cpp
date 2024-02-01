#include <Arduino.h>

void strncpynt(char* dest, const char* src, size_t n) {
  strncpy(dest, src, n);
  dest[n - 1] = '\0';
}