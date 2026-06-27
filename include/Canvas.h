#ifndef CANVAS_H
#define CANVAS_H

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <list>
#include "DisplayScreen.h"
#include "TouchScreen.h"
#include <vector>

#define PORTRAIT_WIDTH 320
#define PORTRAIT_SLICE_HEIGHT 20

class Canvas
{
private:
    // array/matrix for the section of the screen
    // uint16_t _frame_slice[PORTRAIT_WIDTH * PORTRAIT_SLICE_HEIGHT];
    DisplayScreen* _display; // so the pixel changes are visible
    TouchScreen* _touchscreen;
    uint16_t _sliceYStart;
    uint16_t* _dmaBuffer;
    struct Pixel {
        uint16_t x;
        uint16_t y;
    };
    
public:
    std::vector<Pixel> _drawingHistory;
    // constructors
    Canvas(DisplayScreen* frame_buf, TouchScreen* touch); // update the screen with lines drawn

    // methods
    void clearCanvas(uint16_t color); // pick a color like 0xFFFF to fill the screen
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color); // Bresenham's algorithm
    void drawPixel();
    void render();
};

#endif