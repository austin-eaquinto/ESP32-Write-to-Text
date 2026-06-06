#ifndef DISPLAY_H
#define DISPLAY_H

#include "driver/gpio.h"
#include "driver/spi_master.h"

#define D_CS_PIN GPIO_NUM_6
#define D_RESET_PIN GPIO_NUM_4
#define D_DC_PIN GPIO_NUM_17
#define BCKLT_PIN GPIO_NUM_2

class DisplayScreen
{
private:
    spi_device_handle_t _dispHandle;
    gpio_num_t D_CS_Pin;
    gpio_num_t D_RESET_Pin;
    gpio_num_t D_DC_Pin;
    gpio_num_t BCKLT_Pin;

public:
    // constructors
    DisplayScreen();
    DisplayScreen(spi_device_handle_t handle,
                  gpio_num_t csPin = D_CS_PIN,
                  gpio_num_t resetPin = D_RESET_PIN,
                  gpio_num_t dcPin = D_DC_PIN,
                  gpio_num_t bckltPin = BCKLT_PIN);

    // functions
    void begin();
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

#endif