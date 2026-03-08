#include <stdint.h>
#include <stddef.h>
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "serial.h"
#include "pmm.h"
#include "vmm.h"
#include "userspace.h"

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t reserved;
};

void draw_circle(struct multiboot_tag_framebuffer* fb, int cx, int cy, int radius, uint32_t color) {
    // Physical address given by GRUB could be high.
    // If the framebuffer is above 2MB, we'd need to map it in our page tables.
    // QEMU VESA LFB is usually at 0xFD000000.
    // We haven't mapped it! This is a problem! We only mapped first 2MB.
    
    // As a hack for testing, let's map a generic 1GB page for the framebuffer.
    // We will do that in assembly or here. 
    
    uint32_t* buffer = (uint32_t*)fb->framebuffer_addr;
    int pitch = fb->framebuffer_pitch / 4; 

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && (uint32_t)px < fb->framebuffer_width && py >= 0 && (uint32_t)py < fb->framebuffer_height) {
                    buffer[py * pitch + px] = color;
                }
            }
        }
    }
}

void user_test() {
    Serial::write_string("Hello from User Mode!\n");
    while (1);
}

extern "C" void kernel_main(uint32_t magic, uint32_t mb2_info) {
    if (magic != 0x36d76289) {
        return; 
    }

    Serial::init();
    Serial::write_string("Booting clawOS...\n");

    GDT::init();
    PIC::remap(0x20, 0x28);
    IDT::init();

    PMM::init(mb2_info);
    VMM::init();

    struct multiboot_tag_framebuffer* fb = nullptr;
    uint8_t* tags = (uint8_t*)(uint64_t)mb2_info;
    uint32_t total_size = *(uint32_t*)tags;
    
    uint32_t offset = 8;
    while (offset < total_size) {
        struct multiboot_tag* tag = (struct multiboot_tag*)(tags + offset);
        if (tag->type == 0) break;
        if (tag->type == 8) {
            fb = (struct multiboot_tag_framebuffer*)tag;
        }
        offset += (tag->size + 7) & ~7;
    }

    if (fb && fb->framebuffer_bpp == 32) {
        int cx = fb->framebuffer_width / 2;
        int cy = fb->framebuffer_height / 2;
        int radius = 100;
        draw_circle(fb, cx, cy, radius, 0x00FF0000); // Red
    }

    uint8_t* user_stack = (uint8_t*)PMM::alloc_page();
    Userspace::jump_to_user((void*)user_test, (void*)(user_stack + 4096));

    while (true) {
        asm volatile("hlt");
    }
}