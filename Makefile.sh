mkdir build
mkdir build/kernel
mkdir build/kernel/arch
mkdir build/kernel/assembly
mkdir build/libs
mkdir build/libs/stdio
mkdir build/libs/string

mkdir dist

# x86_64-elf-gcc -ffreestanding -c src/include/stdio.h -o build/include/stdio.o
# x86_64-elf-gcc -ffreestanding -c src/include/string.h -o build/include/string.o
# x86_64-elf-gcc -ffreestanding -c src/include/kernel/tty.h -o build/include/kernel/tty.o
x86_64-elf-gcc -ffreestanding -c src/kernel/main.c -o build/kernel/main.o
x86_64-elf-gcc -ffreestanding -c src/kernel/arch/tty.c -o build/kernel/arch/tty.o
# x86_64-elf-gcc -ffreestanding -c src/kernel/arch/vga.h -o build/kernel/arch/vga.o

nasm -f elf64 src/kernel/assembly/main.asm -o build/kernel/assembly/main.o
nasm -f elf64 src/kernel/assembly/main64.asm -o build/kernel/assembly/main64.o
nasm -f elf64 src/kernel/assembly/header.asm -o build/kernel/assembly/header.o

x86_64-elf-gcc -ffreestanding -c src/libs/stdio/printf.c -o build/libs/stdio/printf.o
x86_64-elf-gcc -ffreestanding -c src/libs/stdio/putchar.c -o build/libs/stdio/putchar.o
x86_64-elf-gcc -ffreestanding -c src/libs/stdio/puts.c -o build/libs/stdio/puts.o
x86_64-elf-gcc -ffreestanding -c src/libs/string/memset.c -o build/libs/string/memset.o
x86_64-elf-gcc -ffreestanding -c src/libs/string/memcpy.c -o build/libs/string/memcpy.o
x86_64-elf-gcc -ffreestanding -c src/libs/string/memcmp.c -o build/libs/string/memcmp.o
x86_64-elf-gcc -ffreestanding -c src/libs/string/strlen.c -o build/libs/string/strlen.o
x86_64-elf-gcc -ffreestanding -c src/libs/string/memmove.c -o build/libs/string/memmove.o

x86_64-elf-ld -n -o dist/kernel.bin -T targets/linker.ld build/kernel/assembly/main.o build/kernel/assembly/main64.o build/kernel/assembly/header.o build/kernel/main.o build/kernel/arch/tty.o build/libs/stdio/printf.o build/libs/stdio/putchar.o build/libs/stdio/puts.o build/libs/string/memset.o build/libs/string/memcpy.o build/libs/string/memcmp.o build/libs/string/strlen.o build/libs/string/memmove.o
cp dist/kernel.bin targets/iso/boot/kernel.bin
grub-mkrescue /usr/lib/grub/i386-pc -o dist/kernel.iso targets/iso