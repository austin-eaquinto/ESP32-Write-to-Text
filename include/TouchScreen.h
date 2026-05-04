#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "driver/gpio.h"        // espidf specific (not Arduino). for gpio_num_t
#include "driver/spi_master.h"  // spi bus handling

class TouchScreen
{
private:
    // all caps to match the board labels
    /* because the TouchScreen is set up as input only by an interrupt request 
       this class only needs to know about the _irqPin and _csPin. when the screen
       is touched the isr runs which tells the esp32 to pull T_CS low and send some
       data */
    gpio_num_t T_IRQ_Pin;
    gpio_num_t T_CS_Pin;
    spi_device_handle_t _spiHandle;
    // IRAM_ATTR is a special ESP32 attribute that place the function in internal 
    // RAM for faster execution which is crucial for interrupts
    static void IRAM_ATTR irq_handler(void* arg);
    void handle_touch();    // handles the logic

public:
    // constructor
    TouchScreen(spi_device_handle_t _spiHandle, gpio_num_t irqPin, gpio_num_t csPin);
    // methods
    void begin();
};

#endif