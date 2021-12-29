kernel_source_files := $(shell find src/kernel -name *.c)
kernel_object_files := $(patsubst src/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

x86_64_c_source_files := $(shell find src/kernel -name *.c)
x86_64_c_object_files := $(patsubst src/kernel/%.c, build/kernel/%.o, $(x86_64_c_source_files))


x86_64_asm_source_files := $(shell find src/kernel/assembly -name *.asm)
x86_64_asm_object_files := $(patsubst src/kernel/assembly/%.asm, build/kernel/%.o, $(x86_64_asm_source_files))

x86_64_object_files := $(x86_64_c_object_files) $(x86_64_asm_object_files)

$(kernel_object_files): build/kernel/%.o : src/kernel/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I src/include -ffreestanding $(patsubst build/kernel/%.o, src/kernel/%.c, $@) -o $@
	x86_64-elf-gcc -c -I src/kernel/arch -ffreestanding $(patsubst build/kernel/arch/%.o, src/kernel/arch/%.c, $@) -o $@

$(x86_64_c_object_files): build/kernel/%.o : src/kernel/%.c
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -I src/include -ffreestanding $(patsubst build/kernel/%.o, src/kernel/%.c, $@) -o $@
	x86_64-elf-gcc -c -I src/kernel/arch -ffreestanding $(patsubst build/kernel/arch/%.o, src/kernel/arch/%.c, $@) -o $@
	
$(x86_64_asm_object_files): build/kernel/%.o : src/kernel/assembly/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/kernel/%.o, src/kernel/assembly/%.asm, $@) -o $@

.PHONY: build-x86_64
build-x86_64: $(kernel_object_files) $(x86_64_object_files)
	mkdir -p dist/x86_64 && \
	x86_64-elf-ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(x86_64_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso