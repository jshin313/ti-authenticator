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
void DrawOTP(int x, int y, uint32_t code, uint32_t seconds, uint8_t* currSec, double* dt, char* name);
void moveCursor(int* x, int* y);

#endif

typedef struct node {
    unsigned char* b32key;
    uint8_t* key;
    size_t keylen;
    uint32_t code; // Stores otp code at current moment
    char* name;

    // Linked list esq structure
    struct node*  fwd_ptr;
    struct node*  bck_ptr;

} node_t;

#include "otp.h"
#include "base32.h"


int main(void)
{
    int curr_page = 0; // Index starts at 0
    int num_pages = 1;


    node_t *demo1 = malloc(sizeof(node_t));
    demo1 -> b32key = "JBSWY3DPEHPK3PXP";
    demo1->fwd_ptr = NULL;
    demo1->bck_ptr = NULL;

    int len = strlen(demo1 -> b32key);

    // Input key must be divisible by 8
    if (len % 8 != 0)
        return -1;

    // For every 8 characters in base32, 5 bytes are encoded
    demo1 -> keylen = len / 8 * 5;

    demo1 -> key = malloc(demo1 -> keylen);
    base32_decode(demo1 -> b32key, demo1->key, demo1->keylen);

    node_t *nodeptr = demo1; // Keeps track of what node we're at
    node_t *tailptr = demo1; // Keeps track of last element in list

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

    node_t *tmpptr = demo1;
    int counter = 0;
    while (tmpptr != NULL && counter <= 4)
    {
        tmpptr->code = totp(tmpptr->key, tmpptr->keylen, 30, 6);
        tmpptr = tmpptr->fwd_ptr;
        counter++;
    }
    size_t time_step = 30;
    uint8_t seconds;


    do {
        gfx_FillScreen(1);

        // Print header stuff
        gfx_SetTextScale(1, 2);
        gfx_SetTextFGColor(0xe0); // Red
        gfx_PrintStringXY("TI", 90, 15);
        gfx_SetTextFGColor(0x4a); // Gray
        gfx_PrintStringXY("Authenticator", 120, 15);

        gfx_SetTextScale(1, 1);
        gfx_SetTextFGColor(2); // Black
        gfx_SetTextXY(240, 20);
        gfx_PrintInt(curr_page + 1, 1);
        gfx_PrintStringXY("/", 248, 20);
        gfx_SetTextXY(256, 20);
        gfx_PrintInt(num_pages, 1);

        // Arrow keys to change pages
        gfx_SetColor(0x4a);
        gfx_FillTriangle(30, 20, 40, 30, 40, 10);
        gfx_FillTriangle(55, 20, 45, 30, 45, 10);

        // Only display code everytime it changes
        if ((seconds = rtc_GetSeconds()) % time_step == 0)
        {
            node_t *tmpptr = nodeptr;
            int counter = 0;
            while (tmpptr != NULL && counter < 4)
            {
                tmpptr->code = totp(tmpptr->key, tmpptr->keylen, 30, 6);
                if (1)
                    tmpptr = tmpptr->fwd_ptr;
                else
                    tmpptr = tmpptr->bck_ptr;
                counter++;
            }
        }

        /* Render the otp and circle timer */
        node_t *tmpptr = nodeptr;
        int counter = 0;
        while (tmpptr != NULL && counter < 4)
        {
            tmpptr->code = totp(tmpptr->key, tmpptr->keylen, 30, 6);
            if (1)
                tmpptr = tmpptr->fwd_ptr;
            else
                tmpptr = tmpptr->bck_ptr;

            DrawOTP(30, 50 + counter * 45, demo1->code, seconds, &currSec, &dt, "DEMO");
            counter++;
        }


        

        /* Move cursor based on arrow keys */
        moveCursor(&x, &y);

        /* Render the sprite */
        DrawCursor(x, y);

        /* Copy the buffer to the screen */
        /* Same as gfx_Blit(gfx_buffer) */
        gfx_BlitBuffer();

    } while (kb_Data[6] != kb_Clear);

    gfx_End();

#else
        demo1->code = totp(demo1->key, demo1->keylen, 30, 6);
        printf("%u\n", demo1->code);
#endif

    free(demo1->key);
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

void DrawOTP(int x, int y, uint32_t code, uint32_t seconds, uint8_t* currSec, double* dt, char* name)
{

    /* Print name of code*/
    gfx_SetTextFGColor(2); // Black
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY(name, x, y);

    /* Print OTP code */
    gfx_SetTextScale(2, 2);
    gfx_SetTextXY(x, y + 15);
    gfx_SetTextFGColor(0x1b); // Blue
    gfx_PrintInt(code/1000, 3);
    gfx_SetTextXY(x + 60, y + 15);
    gfx_PrintInt(code % 1000, 3);


    /* Display circle thing */
    gfx_SetColor(0x1b);
    gfx_Circle(x + 240, y + 15 + 5, 10);
    gfx_Line(x + 240, y + 15 - 5, x + 240, y + 15 + 5);


    if (*currSec == seconds) {
        *dt += 0.25; // Decrease this value if needed
    }
    else {
        *dt = 0;
    }
    *currSec = seconds;

    /* gfx_SetTextXY(10, 100); */
    /* gfx_PrintInt(seconds, 2); */

    int8_t clock_y  = 10 * sin(degToRad(12 * seconds + - 90 + *dt));
    int8_t clock_x = 10 * cos(degToRad(12 * seconds + - 90 + *dt));

    gfx_Line(x + 240, y + 15 + 5, x + 240 + clock_x, y + 15 + 5 + clock_y);
    
    /* Fill circle thing */
    gfx_FloodFill(x + 239, y + 15 - 4, 0x1b);

    /* Boundary line thing */
    gfx_SetColor(0xDE);
    gfx_HorizLine(x, y + 10 + 25, x + 225);
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
