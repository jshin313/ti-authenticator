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

typedef struct node {
    char* b32key;
    uint8_t* key;
    size_t keylen;
    uint32_t code; // Stores otp code at current moment
    char* name;

    // Linked list esq structure
    struct node*  fwd_ptr;
    struct node*  bck_ptr;

} node_t;

#ifndef REGULAR
#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <textioc.h>

#define rtc_GetSeconds()        (*((uint8_t*)0xF30000))
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)

void DrawOTP(int x, int y, uint32_t code, uint32_t seconds, uint8_t* currSec, double* dt, char* name);
void insertafter(node_t* ptr1, node_t* ptr2);

void setup_gfx_textio(void);
void switch_keymaps(uint24_t *ids);
void display_keymap_indicator(uint24_t *ids);
#endif

#include "otp.h"
#include "base32.h"


int main(void)
{
    int curr_page = 0; // Index starts at 0
    int num_pages = 1;

    int inputText = 0; // Boolean for inputting text
    int node_count = 0;

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
    node_count++;

    node_t *headptr = demo1; // Keeps track of beginning of list
    node_t *nodeptr = demo1; // Keeps track of what node we're at
    node_t *tailptr = demo1; // Keeps track of last element in list

    node_t *tmpptr;
    
    int counter;

#ifndef REGULAR
	uint24_t *ids;
	sk_key_t key = 0;

    /* Initialize the graphics */
    gfx_Begin();

    /* Draw to the offscreen buffer */
    gfx_SetDrawBuffer();

    /* gfx_FillScreen(0xff); // white */

    double dt = 0;
    uint8_t currSec = 0;

    node_t *demo2 = malloc(sizeof(node_t));
    demo2 -> b32key = "JBSWY3DPEHPK3PLG";
    demo2->fwd_ptr = NULL;
    demo2->bck_ptr = NULL;
    len = strlen(demo2 -> b32key);
    // Input key must be divisible by 8
    if (len % 8 != 0)
        return -1;
    // For every 8 characters in base32, 5 bytes are encoded
    demo2 -> keylen = len / 8 * 5;
    demo2 -> key = malloc(demo2 -> keylen);
    base32_decode(demo2 -> b32key, demo2->key, demo2->keylen);

    insertafter(tailptr, demo2);
    tailptr = demo2;
    node_count++;

    num_pages = ceil(node_count / 4.0);

    tmpptr = headptr;
    counter = 0;
    while (tmpptr != NULL && counter < 4)
    {
        tmpptr->code = totp(tmpptr->key, tmpptr->keylen, 30, 6);
        tmpptr = tmpptr->fwd_ptr;
        counter++;
    }
    size_t time_step = 30;
    uint8_t seconds;

    do {
        gfx_FillScreen(0xff);

        // Print header stuff
        gfx_SetTextScale(1, 2);
        gfx_SetTextFGColor(0xe0); // Red
        gfx_PrintStringXY("TI", 90, 15);
        gfx_SetTextFGColor(0x4a); // Gray
        gfx_PrintStringXY("Authenticator", 120, 15);

        gfx_SetTextScale(1, 1);
        gfx_SetTextFGColor(0); // Black
        gfx_SetTextXY(260, 20);
        gfx_PrintInt(curr_page + 1, 1);
        gfx_PrintStringXY("/", 268, 20);
        gfx_SetTextXY(276, 20);
        gfx_PrintInt(num_pages, 1);

        // Only display code everytime it changes
        if ((seconds = rtc_GetSeconds()) % time_step == 0)
        {
            tmpptr = nodeptr;
            counter = 0;
            while (tmpptr != NULL && counter < 4)
            {
                tmpptr->code = totp(tmpptr->key, tmpptr->keylen, 30, 6);
                tmpptr = tmpptr->fwd_ptr;
                counter++;
            }
        }

        /* Render the otp and circle timer */
        tmpptr = nodeptr;
        int counter = 0;
        while (tmpptr != NULL && counter < 4)
        {
            tmpptr->code = totp(tmpptr->key, tmpptr->keylen, 30, 6);

            DrawOTP(30, 50 + counter * 45, tmpptr->code, seconds, &currSec, &dt, "DEMO");
            tmpptr = tmpptr->fwd_ptr;
            counter++;
        }
        
        inputText = 1;
        if (inputText)
        {
            gfx_SetTextFGColor(0); // Black
            gfx_SetTextScale(1, 1);

            /* Setup TextIOC to use GraphX. */
            setup_gfx_textio();

            /* Create an IDS that will hold 10 characters and use
               the uppercase and lowercase letter keymaps as well
               as the numerical keymap. */
            textio_CreateAlphaNumericalIDS(ids, 10, 200, 200, 50);

            /* Return early if a memory error occured. */
            if (ids == NULL)
                goto ERROR;

            /* Draw a box around the input field. */
            gfx_SetColor(0x00);
            gfx_Rectangle_NoClip(198, 198, 54, 13);

            /* Set the cursor configuration. */
            textio_SetCursorColor(ids, 0x00);
            textio_SetCursorDimensions(ids, 1, 9);
            textio_SetCursorY(ids, 200);
            gfx_BlitBuffer();

            do {
                key = textio_Input(ids);

                if (key == sk_Alpha)
                    switch_keymaps(ids);

                gfx_BlitBuffer();
            } while (key != sk_Enter);

            char * name = textio_GetDataBufferPtr(ids);

            gfx_PrintStringXY(name, 200, 200);

            /* Always delete any IDSes created before the program
               exits. */
            textio_DeleteIDS(ids);
        }

        /* Copy the buffer to the screen */
        /* Same as gfx_Blit(gfx_buffer) */
        gfx_BlitBuffer();

        kb_Scan();

        // Cycle through the pages with left and right arrow
        if (kb_Data[7] & kb_Right && curr_page < num_pages - 1) {
            curr_page++;
            for (int i = 0; i < 4 && nodeptr->fwd_ptr != NULL; i++)
                nodeptr = nodeptr->fwd_ptr;
        } 
        else if (kb_Data[7] & kb_Left && curr_page > 0) {
            curr_page--;
            for (int i = 0; i < 4 && nodeptr->bck_ptr != NULL; i++)
                nodeptr = nodeptr->bck_ptr;
        }

        if (kb_Data[6] & kb_Add) {
            inputText = 1;
        }
        
    } while (kb_Data[6] != kb_Clear);

#else
        demo1->code = totp(demo1->key, demo1->keylen, 30, 6);
        printf("%u\n", demo1->code);
#endif

    tmpptr = headptr;
    counter = 0;
    while (tmpptr != NULL)
    {
        free(tmpptr->key);
        node_t *freethis = tmpptr;
        tmpptr = tmpptr->fwd_ptr;
        free(freethis);
        counter++;
    }

	ERROR:
	gfx_End();
	exit(0);
}


#ifndef REGULAR
// Inserts ptr2 after ptr1
void insertafter(node_t* ptr1, node_t* ptr2)
{
    if (ptr1->fwd_ptr != NULL)
        ptr1->fwd_ptr->bck_ptr = ptr2;

    ptr1->fwd_ptr = ptr2;
    ptr2->bck_ptr = ptr1;
}

void DrawOTP(int x, int y, uint32_t code, uint32_t seconds, uint8_t* currSec, double* dt, char* name)
{

    /* Print name of code*/
    gfx_SetTextFGColor(0); // Black
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

void setup_gfx_textio(void) {

	/* Allocate the pointer structure. */
	textio_library_routines_t *ptr = malloc(sizeof(textio_library_routines_t));

	/* Tell TextIOC that it will be using GraphX. */
	textio_SetSourceLibrary(TEXTIO_SET_GRAPHX_AS_SRC_LIB);

	/* Set the struct's pointers to the requisite GraphX functions. */
	ptr->set_cursor_position = &gfx_SetTextXY;
	ptr->get_cursor_x = &gfx_GetTextX;
	ptr->get_cursor_y = &gfx_GetTextY;
	ptr->draw_char = &gfx_PrintChar;
	ptr->get_char_width = &gfx_GetCharWidth;

	/* Pass the struct pointers to TextIOC. */
	textio_SetLibraryRoutines(ptr);
	
	/* Free the structure memory. */
	free(ptr);
	
	return;
}

void switch_keymaps(uint24_t *ids) {
	
	uint8_t curr_keymap_num;
	
	/* Get the current keymap number */
	curr_keymap_num = textio_GetCurrKeymapNum(ids);
	
	/* Go to the next keymap */
	if (!curr_keymap_num) {
		curr_keymap_num = 2;
	} else {
		/* The number of the first IDS keymap is 0 */
		curr_keymap_num = 0;
	};
	
	textio_SetCurrKeymapNum(ids, curr_keymap_num);
	display_keymap_indicator(ids);
	
	return;
}

void display_keymap_indicator(uint24_t *ids) {
	
	uint24_t cursor_x;
	uint8_t cursor_y;
	char indicator;
	
	/* Get current cursor x-position */
	cursor_x = textio_GetCurrCursorX();
	
	/* Get the cursor's y-position. */
	cursor_y = textio_GetCursorY(ids);
	
	/* Get the character that acts as the current keymap's indicator. */
	indicator = textio_GetCurrKeymapIndicator(ids);
	
	/* Draw the indicator. */
	gfx_SetColor(0x00);
	gfx_FillRectangle_NoClip(cursor_x, cursor_y, gfx_GetCharWidth(indicator) + 2, 9);
	
	gfx_SetTextBGColor(0x00);
	gfx_SetTextFGColor(0xFF);
	gfx_SetTextTransparentColor(0x00);
	gfx_SetTextXY(cursor_x + 1, cursor_y + 1);
	gfx_PrintChar(indicator);
	
	/* Pause */
	delay(200);
	
	/* Set the global color index to the cursor background color. */
	gfx_SetColor(textio_GetCursorBGColor());
	
	/* Erase the indicator. */
	gfx_FillRectangle_NoClip(cursor_x, cursor_y, gfx_GetCharWidth(indicator) + 4, 9);
	
	/* Reset the font colors */
	gfx_SetTextBGColor(0xFF);
	gfx_SetTextFGColor(0x00);
	gfx_SetTextTransparentColor(0xFF);
	
	return;
}
#endif
