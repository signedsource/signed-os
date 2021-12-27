#include "print.h"
// #include "dos.h"

void kernel_main() {
    print_clear();
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    printf("*************************************************************************\n");
    printf("Welcome to ");
    print_set_color(PRINT_COLOR_LIGHT_RED, PRINT_COLOR_BLACK);
    printf("SignedOS\n");
    print_set_color(PRINT_COLOR_WHITE, PRINT_COLOR_BLACK);
    printf("***");
    dos();
}