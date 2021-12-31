#include "../include/stdio.h"
#include "../include/kernel/tty.h"
// #include "../include/string.h"

void kernel_main() {
    terminal_initialize();
    for (int i = 0; i < 100; i++) {
        printf("%c\n", (char) i);
    }

    printf("Curious, this is a test of the printf() function.\n");
    printf("This is a test of the printf() function.\n");
    printf("Y vuelve");
}