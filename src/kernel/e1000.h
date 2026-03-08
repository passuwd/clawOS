#pragma once

#include <stdint.h>
#include "pci.h"

namespace E1000 {

void init(PCI::Device* pci_dev);
void send_packet(void* data, uint16_t length);

} // namespace E1000