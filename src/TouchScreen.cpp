#include "TouchScreen.h"

// constructor
TouchScreen::TouchScreen(spi_device_handle_t handle, gpio_num_t irqPin, gpio_num_t csPin)
    : _spiHandle(handle), T_IRQ_Pin(irqPin), T_CS_Pin(csPin)
{
}

// methods
void TouchScreen::begin()
{

}