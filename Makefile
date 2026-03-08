CXX = g++
CC = gcc
LD = ld

CXXFLAGS = -ffreestanding -mcmodel=kernel -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -O2 -Wall -Wextra -std=c++20 -fno-rtti -fno-exceptions -fno-pic -fno-pie -no-pie
CFLAGS = -m64 -ffreestanding -mcmodel=kernel -mno-red-zone -O2 -Wall -Wextra -fno-pic -fno-pie -no-pie
ASFLAGS32 = -m32
LDFLAGS = -n -T src/linker.ld -m elf_x86_64 -z max-page-size=0x1000

OBJS = src/boot/boot.o src/boot/boot64.o src/kernel/main.o src/kernel/gdt.o src/kernel/gdt_asm.o src/kernel/serial.o src/kernel/idt.o src/kernel/isr.o src/kernel/isr_asm.o src/kernel/pic.o src/kernel/pmm.o src/kernel/vmm.o

clawos.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

src/boot/boot.o: src/boot/boot.S
	$(CC) $(CFLAGS) -c $< -o $@

src/boot/boot64.o: src/boot/boot64.S
	$(CC) $(CFLAGS) -c $< -o $@

src/kernel/main.o: src/kernel/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/gdt.o: src/kernel/gdt.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/gdt_asm.o: src/kernel/gdt_asm.S
	$(CC) $(CFLAGS) -c $< -o $@

src/kernel/serial.o: src/kernel/serial.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/idt.o: src/kernel/idt.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/isr.o: src/kernel/isr.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/isr_asm.o: src/kernel/isr_asm.S
	$(CC) $(CFLAGS) -c $< -o $@

src/kernel/pic.o: src/kernel/pic.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/pmm.o: src/kernel/pmm.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/kernel/vmm.o: src/kernel/vmm.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean run build esp

build: clawos.bin esp

esp: clawos.bin
	mkdir -p esp/EFI/BOOT esp/boot/grub
	cp clawos.bin esp/boot/clawos.bin
	if [ ! -f esp/EFI/BOOT/BOOTX64.EFI ]; then grub-mkstandalone -O x86_64-efi -o esp/EFI/BOOT/BOOTX64.EFI; fi

clean:
	rm -f $(OBJS) clawos.bin
	rm -rf esp/boot/clawos.bin

run: build
	qemu-system-x86_64 -m 256M -vga std -net nic,model=e1000 -net user -d guest_errors,int,cpu_reset -D qemu.log -serial file:serial.log -vnc :0 -drive if=pflash,format=raw,readonly=on,file=/usr/share/qemu/edk2-x86_64-code.fd -drive file=fat:rw:esp,format=raw
