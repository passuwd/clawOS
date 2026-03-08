#include "exfat.h"
#include "serial.h"

namespace ExFAT {

void init() {
    Serial::write_string("exFAT driver initialization...\n");
    // This would initialize the driver and register it with VFS.
}

} // namespace ExFAT