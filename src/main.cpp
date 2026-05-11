#include "TouchScreen.h"

void app_main() {
    spi_device_handle_t spiHandle = NULL;   // this is a pointer
    gpio_num_t IRQPin = GPIO_NUM_NC;        // GPIO_NUM_NC is a safe default value
    gpio_num_t CSPin = GPIO_NUM_NC;

    // ESP-IDF function to start the service that listens for interrupts
    gpio_install_isr_service(0);

    // define the SPI bus
    spi_bus_config_t spi_bus {
        .mosi_io_num = 23,
        .miso_io_num = 19,
        .sclk_io_num = 18,
        // these are used for Quad SPI. by setting them to -1 it tells the
        // ESP-IDF driver that these pins are not connected or used.
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    
    /* -----INITIALIZE THE SPI BUS-----
        -Use ESP_ERROR_CHECK() to make sure the bus is initialized correctly or
        return an error if not. 
        - Returns ESP_OK or prints an error and triggers a restart for safety. 
        - spi_bus_initialize() requires 3 args. SPI Host, pointer to bus 
        configuration, DMA channel. 
        '''''THIS IS THE ROAD''''' */
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &spi_bus, 1));

    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE TOUCHSCREEN↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    // configure the touchscreen ⌄⌄⌄
    spi_device_interface_config_t touch_devcfg {
        .clock_speed_hz = 1 * 1000 * 1000,
        .spics_io_num = 25
    };

    // add touchscreen to the SPI bus
    // 3 args- Host, Address of configured device, Adress of the handle variable
    spi_bus_add_device(HSPI_HOST, &touch_devcfg, &spiHandle);
    
    // instantiate the touchscreen device
    TouchScreen ts(spiHandle, IRQPin, CSPin);
    
    ts.begin();
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/

    // configure the display screen
    // spi_device_interface_config_t display_devcfg {
    //     .clock_speed_hz = ,
    //     .spics_io_num = 5
    // };
}