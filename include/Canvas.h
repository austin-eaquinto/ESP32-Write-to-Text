#ifndef CANVAS_H
#define CANVAS_H

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "DisplayScreen.h"

#define PORTRAIT_WIDTH 320
#define PORTRAIT_SLICE_HEIGHT 80

class Canvas
{
private:
    // array/matrix for the section of the screen
    uint16_t _frame_slice[PORTRAIT_WIDTH * PORTRAIT_SLICE_HEIGHT];
    DisplayScreen* _display; // so the pixel changes are visible
    uint16_t _sliceYStart;

public:
    // constructors
    Canvas();
    Canvas(DisplayScreen* frame_buf); // update the screen with lines drawn

    // methods
    void clearCanvas(uint16_t color); // pick a color like 0xFFFF to fill the screen
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color); // Bresenham's algorithm
    void render();
};

#endif