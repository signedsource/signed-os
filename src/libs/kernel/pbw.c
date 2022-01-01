void pbw(unsigned short port, unsigned char data) {
	asm volatile("outb %0, %1"
        : "=a"(data)
        : "d"(port));
}