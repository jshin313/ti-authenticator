////////////////////////////////////////
// OTP 1.0
// Author: Jacob Shin (deuteriumoxide)
// License: MIT
// Description: 2FA with your calculator!
////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#ifndef REGULAR
#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

#define rtc_GetSeconds()        (*((uint8_t*)0xF30000))
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

/* Include the sprite data */
#include "gfx/gfx.h"

#define START_X ((LCD_WIDTH - cursor_width) / 2)
#define START_Y ((LCD_HEIGHT - cursor_height) / 2)

/* Create a buffer to store the background behind the sprite */
gfx_UninitedSprite(background, cursor_width, cursor_height);

void DrawCursor(int x, int y);
void DrawOTP(int x, int y, uint32_t code, uint32_t seconds, uint8_t* currSec, double* dt);
void moveCursor(int* x, int* y);
#endif

#include "otp.h"
#include "base32.h"


int main(void)
{
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

#ifndef REGULAR

    background->width = cursor_width;
    background->height = cursor_height;

    /* Coordinates used for the sprite */
    int x = START_X;
    int y = START_Y;

    /* Initialize the graphics */
    gfx_Begin();

    /* Set the palette for the sprites */
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_FillScreen(1);
    gfx_SetTransparentColor(0);

    /* Draw to the offscreen buffer */
    gfx_SetDrawBuffer();

    /* Get the original background behind the sprite */
    gfx_GetSprite(background, x, y);

    /* Draw the main sprite */
    DrawCursor(x, y); 
    /* Copy the buffer to the screen */
    /* Same as gfx_Blit(gfx_buffer) */
    gfx_BlitBuffer();


    double dt = 0;
    uint8_t currSec = 0;

    uint32_t code = totp(key, keylen, 30, 6);
    size_t time_step = 30;
    uint8_t seconds;

    do {
        gfx_FillScreen(1);

        gfx_SetTextScale(1, 2);
        gfx_SetTextFGColor(0xe0); // Red
        gfx_PrintStringXY("TI", 90, 24);
        gfx_SetTextFGColor(0x4a); // Gray
        gfx_PrintStringXY("Authenticator", 120, 24);


        // Only display code everytime it changes
        if ((seconds = rtc_GetSeconds()) % time_step == 0)
        {
            code = totp(key, keylen, time_step, 6);
        }
#endif


#ifdef REGULAR
        code = totp(key, keylen, 30, 6);
        printf("%u\n", code);
#else
        /* Render the otp and circle timer */
        DrawOTP(30, 80, code, seconds, &currSec, &dt);

        /* Move cursor based on arrow keys */
        moveCursor(&x, &y);

        /* Render the sprite */
        DrawCursor(x, y);

        /* Copy the buffer to the screen */
        /* Same as gfx_Blit(gfx_buffer) */
        gfx_BlitBuffer();

    } while (kb_Data[6] != kb_Clear);

    gfx_End();

#endif
    free(key);
}

#ifndef REGULAR

/* Function for drawing the main sprite */
void DrawCursor(int x, int y)
{
    static int oldX = START_X;
    static int oldY = START_Y;

    /* Render the original background */
    gfx_Sprite(background, oldX, oldY);

    /* Get the background behind the sprite */
    gfx_GetSprite(background, x, y);

    /* Render the sprite */
    gfx_TransparentSprite(cursor, x, y);

    oldX = x;
    oldY = y;
}

void DrawOTP(int x, int y, uint32_t code, uint32_t seconds, uint8_t* currSec, double* dt)
{
        gfx_SetTextScale(2, 2);

        gfx_SetTextXY(x, y);
        gfx_SetTextFGColor(0x1b); // Blue
        gfx_PrintInt(code/1000, 3);

        gfx_SetTextXY(x + 60, y);
        gfx_PrintInt(code % 1000, 3);


        gfx_SetColor(0x1b);
        gfx_Circle(x + 220, y + 5, 10);

        gfx_Line(x + 220, y - 5, x + 220, y + 5);


        if (*currSec == seconds) {
            *dt += 0.25; // Decrease this value if needed
        }
        else {
            *dt = 0;
        }
        *currSec = seconds;


        /* gfx_SetTextXY(10, 100); */
        /* gfx_PrintInt(seconds, 2); */

        int8_t clock_y = 10 * sin(degToRad(12 * seconds + - 90 + *dt));
        int8_t clock_x = 10 * cos(degToRad(12 * seconds + - 90 + *dt));

        gfx_Line(x + 220, y + 5, x + 220 + clock_x, y + 5 + clock_y);

        gfx_FloodFill(x + 219, y - 4, 0x1b);

        gfx_SetColor(0xDE);
        gfx_HorizLine(x, y + 25, x + 210);
}

/* Taken from the sprite_moving example */
void moveCursor(int* x, int* y)
{
    // Cursor
    kb_key_t arrows;

    /* Scan the keypad to update kb_Data */
    kb_Scan();

    /* Get the arrow key statuses */
    arrows = kb_Data[7];

    /* Check if any arrows are pressed */
    if (arrows)
    {
        /* Do different directions depending on the keypress */
        if (arrows & kb_Right)
        {
            *x += 3;
        }
        if (arrows & kb_Left)
        {
            *x -= 3;
        }
        if (arrows & kb_Down)
        {
            *y += 3;
        }
        if (arrows & kb_Up)
        {
            *y -= 3;
        }
    }
}

#endif
