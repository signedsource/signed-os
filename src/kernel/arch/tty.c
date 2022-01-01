#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "vga.h"

#include "../../include/stdio.h"
#include "../../include/string.h"
#include "../../include/kernel/kernel.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 24;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll() {
    if (terminal_row++ == 24) {
        terminal_column = 0;

        char* screen = (char*) VGA_MEMORY + 320;
        terminal_row = 24;

        for (size_t i = 0; i < 3840; i++) {
            *(screen - 160) = *screen;
            screen++;
        }
    } else {
        terminal_column = 0;
        terminal_row++;
    }
}

void terminal_putchar(char c) {
	unsigned char uc = c;
    if (terminal_column++ == VGA_WIDTH) {
        if (terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        } else {
            terminal_column = 0;
            terminal_row++;
        }
    }

	terminal_putentryat(uc, terminal_color, terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (data[i] == '\n') {
            terminal_scroll();
        } else {
            terminal_putchar(data[i]);
        }
    }	
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}

unsigned char pbr(unsigned short port) {
    unsigned char result;

    __asm__ ( "inb %1, %0": "=a"(result) : "d"(port));

    return result;
} 

char get_key() {
    unsigned char kbd[128] = {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
        '9', '0', '-', '=', '\b',                         /* Backspace */
        0,                                                /* Tab */
        'q', 'w', 'e', 'r',                               /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     /* Enter key */
        0,                                                /* 29   - Control */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
        '\'', '`', 0,                                     /* Left shift */
        '\\', 'z', 'x', 'c', 'v', 'b', 'n',               /* 49 */
        'm', ',', '.', '/', 0,                            /* Right shift */
        '*',
        0,   /* Alt */
        ' ', /* Space bar */
        0,   /* Caps lock */
        0,   /* 59 - F1 key ... > */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, /* < ... F10 */
        0, /* 69 - Num lock*/
        0, /* Scroll Lock */
        0, /* Home key */
        0, /* Up Arrow */
        0, /* Page Up */
        '-',
        0, /* Left Arrow */
        0,
        0, /* Right Arrow */
        '+',
        0, /* 79 - End key*/
        0, /* Down Arrow */
        0, /* Page Down */
        0, /* Insert Key */
        0, /* Delete Key */
        0, 0, 0,
        0, /* F11 Key */
        0, /* F12 Key */
        0, /* All other keys are undefined */
    };

    char keycode = 0;
    char status = 0;
    status = pbr(0x64);
    if (status & 0x01) {
        keycode = pbr(0x60);
    }

    if (keycode >= 128 || keycode < 0)
        return 0;

    return kbd[keycode];
}

void scanf(char *buf, int n) {
    char key = 0;
    int i = 0;
    bool stop = false;

    do {
        key = get_key();
        if (key == 0)
            continue;

        if (key == '\n') {
            terminal_write("\n", sizeof("\n"));
            stop = true;
            break;
        } else if (key == '\b') {
            if (i > 0 && terminal_column > 0) {
                terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
                terminal_column--;
                i--;
                buf--;
            }
        } else {
            if (i < n - 1) {
                *buf++ = key;
                terminal_write(&key, sizeof(key));
                i++;
            }
        }
    } while (stop == false);

    *(buf++) = '\0';
}