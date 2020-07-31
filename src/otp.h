#include <stdint.h>
#include <stdlib.h>

uint32_t totp(uint8_t* key, size_t keylen, size_t timestep, size_t digits);
uint32_t hotp(uint8_t* key, size_t keylen, uint32_t count, size_t digits);
