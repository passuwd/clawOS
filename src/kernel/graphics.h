#pragma once

#include <stdint.h>

namespace Graphics {

struct Color {
    uint8_t b, g, r, a;
};

struct Framebuffer {
    uint32_t* address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
};

void init(Framebuffer fb);
void clear(uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void draw_circle(int cx, int cy, int radius, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);

extern Framebuffer main_fb;

} // namespace Graphics