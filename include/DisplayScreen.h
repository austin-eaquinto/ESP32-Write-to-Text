#ifndef DISPLAY_H
#define DISPLAY_H

#include "driver/gpio.h"
#include "driver/spi_master.h"

class DisplayScreen
{
private:
    gpio_num_t D_CS_Pin;
    gpio_num_t dispHandle;

public:
    // constructors
    DisplayScreen(gpio_num_t handle);
};

#endif