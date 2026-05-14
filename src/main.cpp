#include "TouchScreen.h"

// Constants
#define T_IRQ_PIN GPIO_NUM_26
#define T_DO_PIN GPIO_NUM_19
#define T_DIN_PIN GPIO_NUM_23
#define T_CS_PIN GPIO_NUM_25
#define T_CLK_PIN GPIO_NUM_18
#define DEFAULT_VALUE GPIO_NUM_NC

void app_main() {
    spi_device_handle_t spiHandle = NULL;   // this is a pointer

    // ESP-IDF function to start the service that listens for interrupts
    gpio_install_isr_service(0);

    // define the SPI bus
    spi_bus_config_t spi_bus {
        .mosi_io_num = T_DIN_PIN,
        .miso_io_num = T_DO_PIN,
        .sclk_io_num = T_CLK_PIN,
        // these are used for Quad SPI. by setting them to -1 it tells the
        // ESP-IDF driver that these pins are not connected or used.
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC
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
        .spics_io_num = T_CS_PIN
    };

    // add touchscreen to the SPI bus
    // 3 args- Host, Address of configured device, Adress of the handle variable
    spi_bus_add_device(HSPI_HOST, &touch_devcfg, &spiHandle);
    
    // instantiate the touchscreen device
    TouchScreen ts(spiHandle, T_IRQ_PIN, T_CS_PIN);
    
    ts.begin();
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/

    // configure the display screen
    // spi_device_interface_config_t display_devcfg {
    //     .clock_speed_hz = ,
    //     .spics_io_num = 5
    // };

    while(true) {
        if (TouchScreen::getIsTouched())    // if its true, run this block
        {
            // run ISR for TouchScreen
            // ts.handle_touch();
            // reset flag
            TouchScreen::setIsTouched(false);
        }
    }
}