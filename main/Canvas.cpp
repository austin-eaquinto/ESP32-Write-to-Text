#include "Canvas.h"

// constructors
Canvas::Canvas(DisplayScreen* frame_buf)
    : _display(frame_buf)
{
}

// methods
void Canvas::clearCanvas(uint16_t color)
{
    // create a canvas to draw on
    for(int i = 0; i < (PORTRAIT_WIDTH * PORTRAIT_SLICE_HEIGHT); i++)
    {
        _frame_slice[i] = color;
    }
}

/* Uses Bresenham's algorithm to draw lines. */
void Canvas::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    // abs() returns absolute value of a number. 5 is 5, -7 is 7
    int dx = abs(x1 - x0); // i.e. 319 - 58
    int dy = abs(y1 - y0); // i.e. 79 - 79
    // horizontal step direction
    int sx = x0 < x1 ? 1 : -1;
    // vertical step direction
    int sy = y0 < y1 ? 1 : -1;
    // error tracker. Creates a baseline balance score that determines which pixel and
    // direction the line continues. (Track diagonal drift)
    int err = dx - dy;

    while(true)
    {
        // is y0 >= starting Y coordinate AND < the starting Y coordinate + 80 for the current slice?
        if(y0 >= _sliceYStart && y0 < (_sliceYStart + PORTRAIT_SLICE_HEIGHT))
        {
            // get the starting pixel
            // local because it's only aware of the single slice it's working with, not the whole screen
            int localY = y0 - _sliceYStart;
            // set the starting pixel
            int pixel = (localY * PORTRAIT_WIDTH) + x0;
            // set the pixel to the chosen color
            _frame_slice[pixel] = color;
        }

        // if x's are same AND y's are same, skip the rest
        if(x0 == x1 && y0 == y1) { break; }
        
        // temp variable for math. Copy err value
        int e2 = err;
        // if the error score allows, step horizontally
        if(e2 > -dy)
        {
            // adjust error balance to account for horizontal shift
            err -= dy;
            // move one pixel, left or right, on the X-axis
            x0 += sx;
        }

        // if the error score allows, step vertically
        if(e2 < dx)
        {
            // adjust error balance to account for vertical shift
            err += dx;
            // move one pixel, left or right, on the Y-axis
            y0 += sy;
        }
    }
}

/* WIP */
void Canvas::render()
{
    // clear RAM data
    clearCanvas(0xFFFF);
    // drawLine(x0, y0, x1, y1, color);
    // args- x0,yStart,x1,yEnd
    _display->setAddrWindow(0,_sliceYStart,319,(_sliceYStart + PORTRAIT_SLICE_HEIGHT - 1));
    // ...
}