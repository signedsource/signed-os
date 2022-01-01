#ifndef _KERNEL_H_
#define _KERNEL_H_
#include <stdint.h>

void kabort();
void kerror(char* err);
void pbw(unsigned short port, unsigned char data);

#endif