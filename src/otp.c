////////////////////////////////////////
// OTP 1.0
// Author: Jacob Shin (deuteriumoxide)
// License: MIT
// Description: 2FA on your calculator!
////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef REGULAR
#include <time.h>
#else
#include <tice.h>
#define SECSINYEAR 31536000
#define SECSINLEAPYEAR 31622400

uint32_t getEpochTime(void);
int numLeapYears(uint16_t start, uint16_t end);
int isLeapYear(uint16_t year);
#endif

#include "hmac.h"
#include "base32.h"

uint32_t totp(uint8_t* key, size_t keylen, size_t timestep, size_t digits);
uint32_t hotp(uint8_t* key, size_t keylen, uint32_t count, size_t digits);

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

uint32_t hotp(uint8_t* key, size_t keylen, uint32_t count, size_t digits)
{
    uint8_t digest[20];

    // Convert bytes to an array of bytes
    uint8_t bytes[8] = {0};

    /* bytes[0] = (count >> 56) & 0xFF; */
    /* bytes[1] = (count >> 48) & 0xFF; */
    /* bytes[2] = (count >> 40) & 0xFF; */
    /* bytes[3] = (count >> 32) & 0xFF; */
    bytes[4] = (count >> 24) & 0xFF;
    bytes[5] = (count >> 16) & 0xFF;
    bytes[6] = (count >> 8) & 0xFF;
    bytes[7] = (count) & 0xFF;

    hmac_sha1(digest, key, bytes, keylen, 8);

    // 874977
    /* uint8_t digest[20] = {0x02, 0x57, 0xf4, 0x51, 0x3d, 0xa6, 0xba, 0xd2, 0x84, 0x61, 0x96, 0x44, 0x1e, 0x7c, 0xe7, 0x12, 0x51, 0x5e, 0x96, 0xe6}; */

    /* for (int i = 0 ; i < 20; i++) */
    /* { */
    /*     printf("%02x", digest[i]); */
    /* } */
    /* puts(""); */

    // Truncate digest based on the RFC4226 Standard
    // https://tools.ietf.org/html/rfc4226#section-5.4
    uint32_t offset = digest[19] & 0xf ;
    uint32_t bin_code = (uint32_t)(digest[offset] & 0x7f) << 24
        | (digest[offset+1] & 0xff) << 16
        | (digest[offset+2] & 0xff) <<  8
        | (digest[offset+3] & 0xff);

    // Specification says that the implementation MUST return
    // at least a 6 digit code and possibly a 7 or 8 digit code
    switch (digits)
    {
        case 8:
            return bin_code % 100000000;
        case 7:
            return bin_code % 10000000;
        default:
            return bin_code % 1000000;
    }
}

uint32_t totp(uint8_t* key, size_t keylen, size_t timestep, size_t digits)
{
#ifdef REGULAR
    uint32_t count = time(NULL) / timestep;
#else
    uint32_t count =  getEpochTime() / timestep;
#endif
    return hotp(key, keylen, count, digits);
}

#ifndef REGULAR
uint32_t getEpochTime(void)
{
    uint32_t epochTime = 0;
    // Get time and date in the "normal" Gregorian calendar format
    uint8_t day, month, seconds, minutes, hours;
    uint16_t year;
    int leapYears, i;
    boot_GetDate(&day, &month, &year);
    boot_GetTime(&seconds, &minutes, &hours);

    leapYears = numLeapYears((uint16_t)1970, year);

    epochTime = leapYears * SECSINLEAPYEAR + (year - 1970 - leapYears) * SECSINYEAR;

    for (i = 1; i < month; i++)
    {
        switch (i)
        {
            case 9:
            case 4:
            case 6:
            case 11:
                epochTime += 30 * 3600 * 24;
                break;
            case 2:
                if (isLeapYear(year))
                    epochTime += 29 * 3600 * 24;
                else
                    epochTime += 28 * 3600 * 24;
                break;
            default:
                epochTime += 31 * 3600 * 24;
                break;
        }
    }

    epochTime += (day - 1) * 3600 * 24;
    epochTime += seconds + minutes * 60 + hours * 3600;

    return epochTime;
}

int numLeapYears(uint16_t start, uint16_t end)
{
    int i, count = 0;
    for (i = start; i < end; i++)
    {
        if (isLeapYear(i))
            count++;
    }
    return count;
}

int isLeapYear(uint16_t year)
{
    return ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0));
}
#endif
