#pragma once

#include <stdint.h>

namespace PCI {

struct Device {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t class_id;
    uint8_t subclass_id;
    uint8_t prog_if;
    uint8_t interrupt_line;
    uint32_t bar0;
};

void init();
Device* find_device(uint16_t vendor_id, uint16_t device_id);
uint32_t read_config(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
void write_config(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t value);

} // namespace PCI