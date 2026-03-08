#include "gui.h"
#include "graphics.h"

namespace GUI {

void init() {
    Graphics::clear(0xFF336699); // Windows-like blue background
    draw_taskbar();
    draw_start_button();
}

void draw_taskbar() {
    int h = 40;
    Graphics::draw_rect(0, Graphics::main_fb.height - h, Graphics::main_fb.width, h, 0xFFC0C0C0);
    // Line above taskbar
    Graphics::draw_rect(0, Graphics::main_fb.height - h - 1, Graphics::main_fb.width, 1, 0xFFFFFFFF);
}

void draw_start_button() {
    int h = 30;
    int w = 60;
    int margin = 5;
    int y = Graphics::main_fb.height - 40 + margin;
    Graphics::draw_rect(margin, y, w, h, 0xFFA0A0A0);
    // Logo (red circle) in start button
    Graphics::draw_circle(margin + 15, y + 15, 10, 0xFFFF0000);
}

void update() {
    // Window management logic here
}

} // namespace GUI