#include <peekpoke.h>
#include <atari.h>
#include <stdint.h>

#define SDLSTL_REG ((uint16_t)560U)   
#define SDMCTL_REG ((uint16_t)559U)   
#define CONSOL_REG ((uint16_t)53279U) 

/* 1. Explicitly define arrays with their proper bounds to prevent buffer overflows */
static uint8_t top_mode6_screen[40]; /* 2 rows of Mode 6 text (20 bytes per row) */
static uint8_t btm_mode0_screen[80]; /* 2 rows of Mode 0 text (40 bytes per row) */
static uint8_t custom_dlist[32];     /* Space for custom Display List instructions */

/* 2. A simple conversion function. It converts standard uppercase characters 
      to Atari Internal Screen Codes so they don't look like garbage symbols. */
void write_clean_text(uint8_t* dest, const char* src) {
    uint8_t i = 0;
    while (src[i] != '\0') {
        char c = src[i];
        if (c == ' ') {
            dest[i] = 0; /* Internal Screen Code for Space */
        } else if (c >= 'A' && c <= 'Z') {
            dest[i] = (uint8_t)(c - 32); /* Convert ATASCII 'A'-'Z' to Screen Codes */
        } else if (c >= '0' && c <= '9') {
            dest[i] = (uint8_t)(c - 32); /* Convert numbers to Screen Codes */
        } else if (c == '!') {
            dest[i] = 1;
        } else if (c == '.') {
            dest[i] = 14;
        }
        i++;
    }
}

int main(void) {
    uint16_t top_addr;
    uint16_t btm_addr;
    uint16_t dlist_addr;
    uint16_t orig_dlist_addr;
    uint8_t running;
    uint8_t idx;
    volatile uint16_t delay;

    running = 1;
    idx = 0;

    /* Fill both screens with clear space characters first */
    for (idx = 0; idx < 40; idx++) top_mode6_screen[idx] = 0;
    for (idx = 0; idx < 80; idx++) btm_mode0_screen[idx] = 0;

    /* Write text cleanly to both screens using our conversion mapper */
    write_clean_text(&top_mode6_screen[0], "CAB");
    write_clean_text(&btm_mode0_screen[0], "MODE 0 TEXT IS WORKING!");
    write_clean_text(&btm_mode0_screen[40], "PRESS START TO SAFELY EXIT.");

    /* Get our exact physical memory addresses */
    top_addr = (uint16_t)&top_mode6_screen;
    btm_addr = (uint16_t)&btm_mode0_screen;
    dlist_addr = (uint16_t)&custom_dlist;

    /* Re-assemble our custom display list map safely */
    idx = 0;
    custom_dlist[idx++] = 0x70; /* 8 blank lines */
    custom_dlist[idx++] = 0x70; /* 8 blank lines */
    custom_dlist[idx++] = 0x70; /* 8 blank lines */
    
    /* Top Row: Mode 6 Text Row + LMS */
    custom_dlist[idx++] = 0x46; 
    custom_dlist[idx++] = (top_addr & 0x00FF);        
    custom_dlist[idx++] = ((top_addr >> 8) & 0x00FF); 
    custom_dlist[idx++] = 0x06;                        /* Second row of Mode 6 text */
    
    custom_dlist[idx++] = 0x70; /* Blank divider line */
    
    /* Bottom Row: Mode 0 Text Row + LMS */
    custom_dlist[idx++] = 0x42; 
    custom_dlist[idx++] = (btm_addr & 0x00FF);        
    custom_dlist[idx++] = ((btm_addr >> 8) & 0x00FF); 
    custom_dlist[idx++] = 0x02;                        /* Second row of Mode 0 text */
    
    /* Jump Vector Blank loop configuration */
    custom_dlist[idx++] = 0x41; 
    custom_dlist[idx++] = (dlist_addr & 0x00FF);        
    custom_dlist[idx++] = ((dlist_addr >> 8) & 0x00FF);

    /* Backup and apply the display list to the video chip hardware */
    orig_dlist_addr = PEEK(SDLSTL_REG) | (PEEK(SDLSTL_REG + 1) << 8);

    POKE(SDMCTL_REG, 0); 
    POKE(SDLSTL_REG, dlist_addr & 0x00FF);         
    POKE(SDLSTL_REG + 1, (dlist_addr >> 8) & 0x00FF); 
    POKE(SDMCTL_REG, 34); 

    while (running) {
        if ((PEEK(CONSOL_REG) & 1) == 0) {
            running = 0;
        }
    }

    /* Restore normal OS display list values before program exit */
    POKE(SDMCTL_REG, 0);
    POKE(SDLSTL_REG, orig_dlist_addr & 0x00FF);
    POKE(SDLSTL_REG + 1, (orig_dlist_addr >> 8) & 0x00FF);
    POKE(SDMCTL_REG, 34);

    for (delay = 0; delay < 4000; delay++);

    return 0;
}
