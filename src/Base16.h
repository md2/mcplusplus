#ifndef BASE16_H
#define BASE16_H

#include <unistd.h>
#include <stdint.h>

void base16_encode(char *, const uint8_t *, size_t);
int base16_decode(uint8_t *, const char *, size_t);

#endif

