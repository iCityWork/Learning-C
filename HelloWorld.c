#include <stdio.h>
#include <atari.h>

int main(void) {
    /* Clear screen and print strings using C89 standard compliance */
    printf("%cHello Atari World!\n", 125); /* 125 is ATASCII code to clear screen */
    printf("Compiling with cc65 is working.\n");
    
    /* Keep the program open so the emulator doesn't instantly close it */
    while(1) {
        /* Infinite Loop */
    }
}
