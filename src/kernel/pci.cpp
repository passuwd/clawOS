#include "pci.h"
#include "io.h"
#include "serial.h"

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

namespace PCI {

static Device devices[32];
static int device_count = 0;

uint32_t read_config(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)((uint32_t)bus << 16) | ((uint32_t)device << 11) |
                       ((uint32_t)func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000);
    outl(PCI_CONFIG_ADDR, address);
    return inl(PCI_CONFIG_DATA);
}

void write_config(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)((uint32_t)bus << 16) | ((uint32_t)device << 11) |
                       ((uint32_t)func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000);
    outl(PCI_CONFIG_ADDR, address);
    outl(PCI_CONFIG_DATA, value);
}

void init() {
    Serial::write_string("Scanning PCI bus...\n");
    for (int bus = 0; bus < 256; bus++) {
        for (int dev = 0; dev < 32; dev++) {
            uint32_t v = read_config(bus, dev, 0, 0);
            if ((uint16_t)v == 0xFFFF) continue;
            
            Device d;
            d.bus = bus;
            d.device = dev;
            d.function = 0;
            d.vendor_id = (uint16_t)v;
            d.device_id = (uint16_t)(v >> 16);
            
            uint32_t v2 = read_config(bus, dev, 0, 0x08);
            d.class_id = (v2 >> 24) & 0xFF;
            d.subclass_id = (v2 >> 16) & 0xFF;
            d.prog_if = (v2 >> 8) & 0xFF;
            
            uint32_t v3 = read_config(bus, dev, 0, 0x10);
            d.bar0 = v3;
            
            uint32_t v4 = read_config(bus, dev, 0, 0x3C);
            d.interrupt_line = v4 & 0xFF;
            
            if (device_count < 32) {
                devices[device_count++] = d;
            }
            
            Serial::write_string("Found PCI device: ");
            Serial::write_hex(d.vendor_id);
            Serial::write_string(":");
            Serial::write_hex(d.device_id);
            Serial::write_string("\n");
        }
    }
}

Device* find_device(uint16_t vendor_id, uint16_t device_id) {
    for (int i = 0; i < device_count; i++) {
        if (devices[i].vendor_id == vendor_id && devices[i].device_id == device_id) {
            return &devices[i];
        }
    }
    return nullptr;
}

} // namespace PCI