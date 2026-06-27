#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Canvas.h"

// constructors
Canvas::Canvas(DisplayScreen* frame_buf, TouchScreen* touch)
    : _display(frame_buf), _touchscreen(touch)
{
    _dmaBuffer = (uint16_t*) heap_caps_malloc(12800, MALLOC_CAP_DMA);
}

// methods
void Canvas::clearCanvas(uint16_t color)
{
    // create a canvas to draw on
    for(int i = 0; i < (PORTRAIT_WIDTH * PORTRAIT_SLICE_HEIGHT); i++)
    {
        _dmaBuffer[i] = color;
    }
}

/* A test function that draws a single line on the screen. 
    Uses Bresenham's algorithm to draw lines. */
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
            _dmaBuffer[pixel] = color;
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

void Canvas::drawPixel()
{
    Pixel newPixel;
    newPixel.x = _touchscreen->get_X();
    newPixel.y = _touchscreen->get_Y();
    _drawingHistory.push_back(newPixel);
}

/* 'Conveyor belt' that sends the display data to the screen */
void Canvas::render()
{
    for(_sliceYStart = 0; _sliceYStart < 480; _sliceYStart += PORTRAIT_SLICE_HEIGHT)
    {
        /* clear RAM data */
        // the drawing area
        if (_sliceYStart < 440)
        {
            clearCanvas(0xFFFF);
            for (const auto& point : _drawingHistory)
            {
                if (point.y >= _sliceYStart && point.y < (_sliceYStart + PORTRAIT_SLICE_HEIGHT))
                {
                    int drawY = point.y - _sliceYStart;
                    int drawPixels = (drawY * PORTRAIT_WIDTH) + point.x;
                    _dmaBuffer[drawPixels] = 0x000;
                }
            }
        }
        // the reset/clear button
        if (_sliceYStart >= 440) { clearCanvas(0x3333); }
        
        // drawLine(10, 10, 300, 400, 0x0000); // test line
        // args- x0,yStart,x1,yEnd
        _display->setAddrWindow(0,_sliceYStart,319,(_sliceYStart + PORTRAIT_SLICE_HEIGHT - 1));
        _display->sendDataBlock(_dmaBuffer, PORTRAIT_WIDTH * PORTRAIT_SLICE_HEIGHT);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}