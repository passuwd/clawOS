#include <stdint.h>
#include <stddef.h>
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "serial.h"
#include "pmm.h"
#include "vmm.h"
#include "userspace.h"
#include "pci.h"
#include "e1000.h"
#include "graphics.h"
#include "gui.h"

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

    PCI::init();
    PCI::Device* e1000_dev = PCI::find_device(0x8086, 0x100E);
    if (e1000_dev) {
        E1000::init(e1000_dev);
    }

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
        Graphics::Framebuffer graphics_fb;
        graphics_fb.address = (uint32_t*)fb->framebuffer_addr;
        graphics_fb.width = fb->framebuffer_width;
        graphics_fb.height = fb->framebuffer_height;
        graphics_fb.pitch = fb->framebuffer_pitch;
        
        Graphics::init(graphics_fb);
        GUI::init();
    }

    uint8_t* user_stack = (uint8_t*)PMM::alloc_page();
    Userspace::jump_to_user((void*)user_test, (void*)(user_stack + 4096));

    while (true) {
        asm volatile("hlt");
    }
}