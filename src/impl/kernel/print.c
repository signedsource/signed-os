#include "print.h"
#include "utils.h"
#include "chars.h"
#include <stdbool.h>
// #include <string.h>

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;
struct Char
{
	uint8_t character;
	uint8_t color;
};

bool capslock = false;
bool shift = false;
bool alt = false;
bool ctrl = false;
bool numlock = true;
bool scrolllock = false;

struct Char *volatile buffer = (struct Char *)0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;
char *char_mem;
size_t input_pos = 0;
size_t block_del_pos = 0;
void clear_row(size_t row)
{
	struct Char empty = (struct Char){
		character : ' ',
		color : color,
	};

	for (size_t col = 0; col < NUM_COLS; col++)
	{
		buffer[col + NUM_COLS * row] = empty;
	}
}
void clear_col(size_t column)
{
	column--;
	if (input_pos <= block_del_pos)
	{
	}
	else
	{
		col--;
		struct Char empty = (struct Char){
			character : ' ',
			color : color,
		};
		buffer[column + NUM_COLS * row] = empty;
		input_pos--;
	}
}

void print_clear()
{
	for (size_t i = 0; i < NUM_ROWS; i++)
	{
		clear_row(i);
	}
}

void print_newline()
{
	col = 0;

	if (row < NUM_ROWS)
	{
		row++;
		return;
	}

	for (size_t row = 0; row < NUM_ROWS; row++)
	{
		for (size_t col = 0; col < NUM_COLS; col++)
		{
			struct Char character = buffer[col + NUM_COLS * row];
			buffer[col + NUM_COLS * (row - 1)] = character;
		}
	}

	clear_row(NUM_ROWS - 1);
}

void print_char(char character)
{
	if (character == '\n')
	{
		print_newline();
		return;
	}

	if (col > NUM_COLS)
	{
		print_newline();
	}

	buffer[col + NUM_COLS * row] = (struct Char){
		character : (uint8_t)character,
		color : color,
	};
	input_pos = col + NUM_COLS * row;
	col++;
}

void print_str(char *str)
{
	for (size_t i = 0; 1; i++)
	{
		*(char_mem + 1) = str;
		char character = (uint8_t)str[i];

		if (character == '\0')
		{
			return;
		}
		print_char(character);
	}
}

void print_int(int num)
{
	char str_num[digit_count(num) + 1];
	itoa(num, str_num);
	print_str(str_num);
}

uint8 inb(uint16 port)
{
	uint8 ret;
	asm volatile("inb %1, %0"
				 : "=a"(ret)
				 : "d"(port));
	return ret;
}

void outb(uint16 port, uint8 data)
{
	asm volatile("outb %0, %1"
				 : "=a"(data)
				 : "d"(port));
}

char get_input_keycode() {
	char ch = 0;
	while ((ch = inb(KEYBOARD_PORT)) != 0)
	{
		if (ch > 0)
			return ch;
	}
	return ch;
}

/*
keep the cpu busy for doing nothing(nop)
so that io port will not be processed by cpu
here timer can also be used, but lets do this in looping counter
*/
void wait_for_io(uint32 timer_count)
{
	while (1)
	{
		asm volatile("nop");
		timer_count--;
		if (timer_count <= 0)
			break;
	}
}
void sleep(uint32 timer_count) {
	wait_for_io(timer_count);
}

char strcnt(char str[]) {
	for(int i = 0; str[i] != '\0'; ++i) {
	    print_char(str[i]);
	}
}

void input(void (fn)(char)) {
	char chptr[256] = { "\0" };
	int chptrindex = 0;
	char ch = 0;
	char keycode = 0;
	bool stop = false;
	do {
		keycode = get_input_keycode();
		if (keycode == KEY_ENTER)
		{
			stop = true;
		}
		else if (keycode == KEY_BACKSPACE)
		{
			clear_col(col);
			chptrindex--;
			chptr[chptrindex] = NULL;
		}
		else if (keycode == KEY_LEFT_ALT_PRESS)
		{
			//print_str("LEFT ALT");
			alt = true;
		}
		else if (keycode == KEY_LEFT_SHIFT_PRESS)
		{
			//print_str("LEFT SHIFT");
			shift = true;
		}
		else if (keycode == KEY_LEFT_CTRL_PRESS)
		{
			//print_str("LEFT CTRL");
			ctrl = true;
		}
		else if (keycode == KEY_LEFT_ALT_RELEASE)
		{
			//print_str("LEFT ALT UP");
			alt = false;
		}
		else if (keycode == KEY_LEFT_SHIFT_RELEASE)
		{
			//print_str("LEFT SHIFT UP");
			shift = false;
		}
		else if (keycode == KEY_LEFT_CTRL_RELEASE)
		{
			//print_str("LEFT ALT UP");
			ctrl = false;
		}
		else if (keycode == KEY_CAPS_LOCK_PRESS)
		{
			//print_str("CAPS LOCK");
			capslock = !capslock;
		}
		else
		{
			if (capslock || shift)
			{
				//we're capitalized
				ch = get_ascii_char(keycode);
				chptr[chptrindex] = get_ascii_char(keycode);
				chptrindex++;
			}
			else
			{
				ch = get_ascii_char_lower(keycode);
				chptr[chptrindex] = get_ascii_char_lower(keycode);
				chptrindex++;
			}

			print_char(ch);
		}
		sleep(0x02FFFFFF);
	} while (stop == false);

	fn(strcnt(chptr));
}
/*
char return_input()
{
  static int MAX_INPUT_LEN = 20;
  char ch = 0;
  char chs[MAX_INPUT_LEN];
  char keycode = 0;
  for (size_t i = 0; i < MAX_INPUT_LEN; i++)
  {
    if(keycode == KEY_ENTER){
      print_newline();
      return chs;
    }else{
      ch = get_ascii_char(keycode);
      print_char(ch);
      chs[i] = ch;
    }
    sleep(0x02FFFFFF);
  }
  return chs;
  
}*/

void print_set_color(uint8_t foreground, uint8_t background)
{
	color = foreground + (background << 4);
}

typedef __builtin_va_list va_list;
#define va_start(ap, X) __builtin_va_start(ap, X)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

void _putc(unsigned char c) { print_char((char)c); }

void printf(const char* fmt, ...) {
    const char digits[] = "0123456789abcdef";
    va_list ap;
    char buf[10];
    char* s;
    unsigned char u;
    int c, i, pad;

    va_start(ap, fmt);
    while ((c = *fmt++) != 0) {
        if (c == '%') {
            c = *fmt++;
            if (c == 'l')
                c = *fmt++;
            switch (c) {
            case 'c':
                _putc(va_arg(ap, int));
                continue;
            case 's':
                s = va_arg(ap, char*);
                if (s == (void*)0)
                    s = (char*)"<NULL>";
                for (; *s; s++)
                    _putc((int)*s);
                continue;
            case '%':
                _putc('%');
                continue;
            case 'd':
            case 'i':
                c = 'u';
            case 'u':
            case 'x':
            case 'X':
                u = va_arg(ap, unsigned);
                s = buf;
                if (c == 'u') {
                    do
                        *s++ = digits[u % 10U];
                    while (u /= 10U);
                } else {
                    pad = 0;
                    for (i = 0; i < 8; i++) {
                        if (pad)
                            *s++ = '0';
                        else {
                            *s++ = digits[u % 16U];
                            if ((u /= 16U) == false)
                                pad = 1;
                        }
                    }
                }
                while (--s >= buf)
                    _putc((int)*s);
                continue;
            }
        }
        _putc((int)c);
    }
    va_end(ap);
}