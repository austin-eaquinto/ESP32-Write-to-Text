#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TouchScreen.h"
#include <stdio.h>

// Constants
#define T_IRQ_PIN GPIO_NUM_34
#define T_DO_PIN GPIO_NUM_19
#define T_DIN_PIN GPIO_NUM_23
#define T_CS_PIN GPIO_NUM_25
#define T_CLK_PIN GPIO_NUM_18
#define DEFAULT_VALUE GPIO_NUM_NC
#define D_CS_PIN GPIO_NUM_5

extern "C" void app_main() {
    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE SPI BUS↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    spi_device_handle_t touchHandle = NULL;   // this is a pointer
    spi_device_handle_t displayHandle = NULL;

    // ESP-IDF function to start the service that listens for interrupts
    gpio_install_isr_service(0);
    vTaskDelay(pdMS_TO_TICKS(50)); // delay here to give the system time to first setup correctly

    // define the SPI bus
    spi_bus_config_t spi_bus = {};
    spi_bus.mosi_io_num = T_DIN_PIN;
    spi_bus.miso_io_num = T_DO_PIN;
    spi_bus.sclk_io_num = T_CLK_PIN;
    // these are used for Quad SPI. by setting them to -1 it tells the
    // ESP-IDF driver that these pins are not connected or used.
    spi_bus.quadwp_io_num = GPIO_NUM_NC;
    spi_bus.quadhd_io_num = GPIO_NUM_NC;
    
    /* -----INITIALIZE THE SPI BUS-----
        -Use ESP_ERROR_CHECK() to make sure the bus is initialized correctly or
        return an error if not. 
        - Returns ESP_OK or prints an error and triggers a restart for safety. 
        - spi_bus_initialize() requires 3 args. SPI Host, pointer to bus 
        configuration, DMA channel. 
        '''''THIS IS THE ROAD''''' */
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &spi_bus, 1));
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/


    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE TOUCHSCREEN↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    // configure the touchscreen ⌄⌄⌄
    spi_device_interface_config_t touch_devcfg = {};
    touch_devcfg.clock_speed_hz = 1 * 1000 * 1000;
    touch_devcfg.spics_io_num = T_CS_PIN;
    //
    touch_devcfg.queue_size = 7;

    // add touchscreen to the SPI bus
    // 3 args- Host, Address of configured device, Adress of the handle variable
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &touch_devcfg, &touchHandle));
    
    // instantiate the touchscreen device
    TouchScreen ts(T_IRQ_PIN, T_CS_PIN, touchHandle);
    ts.begin();
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/


    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE DISPLAY SCREEN↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    // configure the display screen
    spi_device_interface_config_t display_devcfg = {};
    display_devcfg.clock_speed_hz = 400000;
    display_devcfg.spics_io_num = D_CS_PIN;
    // display_devcfg.clock_source = ;
    // display_devcfg.command_bits = ;
    // display_devcfg.

    // add display screen to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &display_devcfg, &displayHandle));
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/


    while(true) {
        if (ts.screenTouched())
        {
            printf("X: %u, Y: %u\n", ts.get_X(), ts.get_Y());
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}