////////////////////////////////////////
// OTP 1.0
// Author: Jacob Shin (deuteriumoxide)
// License: MIT
// Description: 2FA on your calculator!
////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef REGULAR
#include <tice.h>
#endif

#include "otp.h"
#include "base32.h"


int main(void)
{
#ifndef REGULAR
    char output[255];
    os_ClrHome();
#endif

    uint8_t* key;
    size_t keylen;

    char* b32key = "JBSWY3DPEHPK3PXP";
    int len = strlen(b32key);

    // Input key must be divisible by 8
    if (len % 8 != 0)
        return -1;

    // For every 8 characters in base32, 5 bytes are encoded
    keylen = strlen(b32key) / 8 * 5;

    key = malloc(keylen);
    base32_decode(b32key, key, keylen);

    uint32_t code = totp(key, keylen, 30, 6);

#ifdef REGULAR
    printf("%u\n", code);
#else
    sprintf(output, "%u", code);
    os_PutStrFull(output);

    while(!os_GetCSC());
#endif
}

