#include "../../include/stdio.h"
#include "../../include/kernel/kernel.h"

void kerror(char* err) {
    printf("%c\n", err);
    kabort();
}