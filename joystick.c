#include <conio.h>
#include <stdint.h>
#include <joystick.h>
#include <atari.h>
#include <peekpoke.h>

/* Cast register constants to unsigned int to remove the signed long warning */
#define CONSOL ((uint16_t)53279U) // $D40F - Console Keys Register  

int main(void) {
    /* All variables declared strictly at the top of the main block */
    unsigned char joy_state;
    volatile unsigned int delay;
    unsigned char running;

    running = 1;
    
    /* Clear Screen using optimized conio function */
    clrscr();
    
    /* Display static header elements once */
    cputs("Joystick 1 Test Program\r\n");
    cputs("Press START to Exit\r\n");
    cputs("-----------------------\r\n");

    /* Install the built-in static joystick driver */
    joy_install(joy_static_stddrv);

    while (running) {
        /* Read joystick 0 (Port 1) */
        joy_state = joy_read(JOY_1);

        /* Explicitly lock cursor to Column 0, Row 4 to prevent text scrolling */
        gotoxy(0, 4); 

        /* Print current joystick state values */
        cprintf("Raw State: %3d | Status: ", joy_state);

        /* Evaluate movement directions */
        if (JOY_UP(joy_state))         cputs("UP   ");
        else if (JOY_DOWN(joy_state))  cputs("DOWN ");
        else if (JOY_LEFT(joy_state))  cputs("LEFT ");
        else if (JOY_RIGHT(joy_state)) cputs("RIGHT");
        else                           cputs("IDLE ");

        /* Lock cursor to a separate column on the same row for button feedback */
        gotoxy(0, 6);
        if (JOY_BTN_1(joy_state)) {
            cputs("Button Action: FIRE! ");
        } else {
            cputs("Button Action: ----- ");
        }
        
        /* Check if the physical START key is pressed to exit safely */
        if ((PEEK(CONSOL) & 1) == 0) {
            running = 0;
        }

        /* Standard processing timing loop */
        for (delay = 0; delay < 2000; delay++);
    }

    /* Reachable uninstallation steps */
    joy_uninstall();
    
    clrscr();
    cputs("Driver uninstalled. Program closed.");
    return 0;
}
