#include "e1000.h"
#include "serial.h"
#include "pmm.h"
#include "vmm.h"
#include "io.h"

namespace E1000 {

static uint64_t mmio_base = 0;
static uint8_t mac[6];

struct rx_desc {
    uint64_t addr;
    uint16_t len;
    uint16_t checksum;
    uint8_t status;
    uint8_t errors;
    uint16_t special;
} __attribute__((packed));

struct tx_desc {
    uint64_t addr;
    uint16_t len;
    uint8_t cso;
    uint8_t cmd;
    uint8_t status;
    uint8_t css;
    uint16_t special;
} __attribute__((packed));

static rx_desc* rx_descs;
static tx_desc* tx_descs;
static uint8_t* rx_buffers;
static uint8_t* tx_buffers;

static void write_reg(uint32_t reg, uint32_t val) {
    *(volatile uint32_t*)(mmio_base + reg) = val;
}

static uint32_t read_reg(uint32_t reg) {
    return *(volatile uint32_t*)(mmio_base + reg);
}

void init(PCI::Device* pci_dev) {
    mmio_base = pci_dev->bar0 & 0xFFFFFFF0;
    // Map MMIO
    for (int i = 0; i < 32; i++) {
        VMM::map_page(VMM::current_pml4, 0xffffffffc0000000 + i*4096, mmio_base + i*4096, VMM::PAGE_WRITE);
    }
    mmio_base = 0xffffffffc0000000;
    
    Serial::write_string("E1000 MMIO mapped at: ");
    Serial::write_hex(mmio_base);
    Serial::write_string("\n");

    // Enable bus mastering
    uint32_t pci_command = PCI::read_config(pci_dev->bus, pci_dev->device, pci_dev->function, 0x04);
    pci_command |= (1 << 2); // Bus Master
    PCI::write_config(pci_dev->bus, pci_dev->device, pci_dev->function, 0x04, pci_command);

    // Read MAC
    uint32_t mac_low = read_reg(0x5400);
    uint32_t mac_high = read_reg(0x5404);
    mac[0] = mac_low & 0xFF;
    mac[1] = (mac_low >> 8) & 0xFF;
    mac[2] = (mac_low >> 16) & 0xFF;
    mac[3] = (mac_low >> 24) & 0xFF;
    mac[4] = mac_high & 0xFF;
    mac[5] = (mac_high >> 8) & 0xFF;

    Serial::write_string("MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial::write_hex(mac[i]);
        if (i < 5) Serial::write_string(":");
    }
    Serial::write_string("\n");

    // Initialize RX/TX queues (simplified)
    // We would allocate memory from PMM and map it here.
    // For now, let's just log progress.
    Serial::write_string("E1000 initialized.\n");
}

} // namespace E1000