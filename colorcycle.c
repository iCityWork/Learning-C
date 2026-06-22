#include <conio.h>
#include <peekpoke.h>
#include <atari.h>
#include <stdint.h>

/* Define the Operating System Color Shadow Registers */
#define COLOR1_SHAD ((uint16_t)709U)   /* Playfield text/graphics shadow register */
#define COLOR2_SHAD ((uint16_t)710U)   /* Screen background shadow register */
#define COLOR4_SHAD ((uint16_t)712U)   /* Border color shadow register */
#define CONSOL_REG  ((uint16_t)53279U)  /* Console keys (Start/Select/Option) */

int main(void) {
    /* Variables strictly declared at the absolute top of the block scope */
    uint8_t color_value;
    uint8_t running;
    volatile uint16_t delay;

    color_value = 0;
    running = 1;

    /* Initialize conio screen setup */
    clrscr();
    cputs("=== Lesson 2: Color Cycling ===\r\n");
    cputs("Press START to safely exit\r\n");
    cputs("-------------------------------\r\n");

    while (running) {
        /* Print current loop telemetry cleanly to fixed screen lines */
        gotoxy(0, 5);
        cprintf("Current Hex Hue: 0x%02X", color_value);

        /* POKE values to the OS Shadow registers so they persist across VBIs */
        POKE(COLOR2_SHAD, color_value);       /* Modifies main background canvas */
        POKE(COLOR4_SHAD, (uint8_t)~color_value); /* Inverts color bits for a distinct border */

        /* Shift the high nibble to step through different color hues smoothly */
        color_value += 2;

        /* Verify if the user pressed the physical START console key to break out */
        if ((PEEK(CONSOL_REG) & 1) == 0) {
            running = 0;
        }

        /* Timing throttle to let human eyes process the shifting palette */
        for (delay = 0; delay < 1500; delay++);
    }

    /* Restore standard Atari system blue/white colors upon cleanup */
    POKE(COLOR2_SHAD, 148); /* System Blue */
    POKE(COLOR4_SHAD, 0);   /* Black Border */
    
    clrscr();
    cputs("Palette test complete.\r\n");
    return 0;
}
