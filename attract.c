#include <conio.h>
#include <peekpoke.h>
#include <atari.h>
#include <stdint.h>

/* Define target system registers with strict warning protection macros */
#define ATTRACT_REG ((uint16_t)77U)     /* Attract mode: 0 = normal, >0 = color cycling active */
#define CONSOL_REG  ((uint16_t)53279U)  /* Console switch key state register */

int main(void) {
    /* Declare all internal variables at the absolute top of the function */
    uint8_t console_state;
    uint8_t running;
    volatile uint16_t delay;

    running = 1;

    clrscr();
    cputs("=== Lesson 2: Hardware Control ===\r\n");
    cputs("Press START to cleanly terminate\r\n");
    cputs("----------------------------------\r\n");

    while (running) {
        /* Force Attract Mode to 0 every loop iteration to lock down steady color states */
        POKE(ATTRACT_REG, 0);

        /* Read the physical current binary snapshot of the console switches */
        console_state = PEEK(CONSOL_REG);

        /* Isolate and lock cursor positioning to line 4 */
        gotoxy(0, 4);
        cprintf("Raw Console Register: %3d\r\n", console_state);

        /* Mask distinct bits: Bit 0=Start, Bit 1=Select, Bit 2=Option. (Low/0 means pressed) */
        gotoxy(0, 6);
        if ((console_state & 2) == 0) {
            cputs("Current Pressed Key: [ SELECT ] ");
        } else if ((console_state & 4) == 0) {
            cputs("Current Pressed Key: [ OPTION ] ");
        } else {
            cputs("Current Pressed Key: [  NONE  ] ");
        }

        /* Check the exit break condition (START key pressed) */
        if ((console_state & 1) == 0) {
            running = 0;
        }

        /* Mechanical loop step delay */
        for (delay = 0; delay < 1000; delay++);
    }

    clrscr();
    cputs("Hardware check finalized successfully.\r\n");
    return 0;
}