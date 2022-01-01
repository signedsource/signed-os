#include <stdbool.h>
#include "../include/stdio.h"
#include "../include/kernel/tty.h"

void kernel_main() {
    terminal_initialize();
    while (true) {
        printf("DOS> ");
        char data[4096];
        scanf(data, 4096);
    }
}