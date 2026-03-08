#include "serial.h"
#include "io.h"

#define PORT 0x3f8 // COM1

namespace Serial {

void init() {
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

void write_char(char a) {
    while (is_transmit_empty() == 0);
    outb(PORT, a);
}

void write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        write_char(str[i]);
    }
}

void write_hex(unsigned long val) {
    write_string("0x");
    for (int i = 15; i >= 0; i--) {
        int nibble = (val >> (i * 4)) & 0xF;
        if (nibble < 10) {
            write_char('0' + nibble);
        } else {
            write_char('A' + (nibble - 10));
        }
    }
}

} // namespace Serial