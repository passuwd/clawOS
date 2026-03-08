#include "graphics.h"

namespace Graphics {

Framebuffer main_fb;

void init(Framebuffer fb) {
    main_fb = fb;
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || (uint32_t)x >= main_fb.width || y < 0 || (uint32_t)y >= main_fb.height) return;
    main_fb.address[y * (main_fb.pitch / 4) + x] = color;
}

void clear(uint32_t color) {
    for (uint32_t y = 0; y < main_fb.height; y++) {
        for (uint32_t x = 0; x < main_fb.width; x++) {
            draw_pixel(x, y, color);
        }
    }
}

void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}

void draw_circle(int cx, int cy, int radius, uint32_t color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                // Simplified anti-aliasing could go here.
                draw_pixel(cx + x, cy + y, color);
            }
        }
    }
}

} // namespace Graphics