#include <stdbool.h>

void kabort() { 
    while (true) { 
        asm ("hlt"); 
    } 
}