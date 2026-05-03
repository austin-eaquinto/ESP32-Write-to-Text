#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "driver/gpio.h" // espidf specific (not Arduino). for gpio_num_t

class TouchScreen
{
private:
    // all caps to match the board labels
    gpio_num_t T_IRQPin;
    gpio_num_t T_DOPin;
    gpio_num_t T_DINPin;
    gpio_num_t T_CSPin;
    gpio_num_t T_CLKPin;

public:
    // constructor
    TouchScreen(gpio_num_t irq, gpio_num_t do_pin, gpio_num_t din_pin, gpio_num_t cs, gpio_num_t clk);

    void initialize();
    void clear();
};

#endif