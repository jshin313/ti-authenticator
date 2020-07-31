#include "otp.h"
#include "hmac.h"

#ifdef REGULAR
#include <time.h>
#else
#include "time.h"
#endif

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
