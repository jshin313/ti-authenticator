////////////////////////////////////////
// OTP 1.0
// Author: Jacob Shin (deuteriumoxide)
// License: MIT
// Description: 2FA on your calculator!
////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifndef REGULAR
#include <tice.h>
#include <graphx.h>
#define rtc_GetSeconds()        (*((uint8_t*)0xF30000))
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

#endif

#include "otp.h"
#include "base32.h"


int main(void)
{
#ifndef REGULAR
    gfx_Begin();

    gfx_SetDrawBuffer();

    double dt = 0;
    uint8_t currSec = 0;

    do {
        gfx_FillScreen(0xFF);
        gfx_SetTextScale(2,2);
        gfx_SetTextFGColor(0xe0);
        gfx_PrintStringXY("TI", 40, 24);
        gfx_SetTextFGColor(0);
        gfx_PrintStringXY("Authenticator", 80, 24);


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


        gfx_SetTextXY(40, 60);
        gfx_SetTextFGColor(0x13);
        gfx_PrintInt(code/1000, 3);

        gfx_SetTextXY(100, 60);
        gfx_PrintInt(code%1000, 3);

        gfx_SetColor(0x13);
        gfx_Circle(230, 65, 10);

        gfx_Line(230, 65- 10, 230, 65);

        // Change this to make a clock
        uint8_t seconds = rtc_GetSeconds();

        if (currSec == seconds) {
            dt += 3;
        }
        else {
            dt = 0;
        }
        currSec = seconds;


        //gfx_SetTextXY(10, 100);
        //gfx_PrintInt(seconds, 2);


        int8_t y = 10 * sin(degToRad(12 * seconds + - 90 + dt));
        int8_t x = 10 * cos(degToRad(12 * seconds + - 90 + dt));


        gfx_Line(230, 65, 230 + x, 65 + y);

        gfx_FloodFill(229, 56, 0x13);

        gfx_BlitBuffer();

    } while(!os_GetCSC());

    gfx_End();

#endif
    }
