#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TouchScreen.h"
#include "DisplayScreen.h" 
#include "Canvas.h"
#include <stdio.h>

// Constants
#define MISO_PIN GPIO_NUM_11
#define MOSI_PIN GPIO_NUM_13
#define MASTER_CLK_PIN GPIO_NUM_12
#define DEFAULT_VALUE GPIO_NUM_NC

extern "C" void app_main() {
    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE SPI BUS↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    spi_device_handle_t touchHandle = NULL;   // this is a pointer
    spi_device_handle_t displayHandle = NULL;

    // ESP-IDF function to start the service that listens for interrupts
    gpio_install_isr_service(0);
    vTaskDelay(pdMS_TO_TICKS(50)); // delay here to give the system time to first setup correctly

    // define the SPI bus
    spi_bus_config_t spi_bus = {};
    spi_bus.mosi_io_num = MOSI_PIN;
    spi_bus.miso_io_num = MISO_PIN;
    spi_bus.sclk_io_num = MASTER_CLK_PIN;
        // these are used for Quad SPI. by setting them to -1 it tells the
        // ESP-IDF driver that these pins are not connected or used.
    spi_bus.quadwp_io_num = GPIO_NUM_NC;
    spi_bus.quadhd_io_num = GPIO_NUM_NC;
    // manually sets the ESP-IDF driver cap. default cap = 4096 bytes
    spi_bus.max_transfer_sz = 320 * 20 * sizeof(uint16_t); // 12800 bytes
    
    /* -----INITIALIZE THE SPI BUS-----
        -Use ESP_ERROR_CHECK() to make sure the bus is initialized correctly or
        return an error if not. 
        - Returns ESP_OK or prints an error and triggers a restart for safety. 
        - spi_bus_initialize() requires 3 args. SPI Host, pointer to bus 
        configuration, DMA channel. 
        '''''THIS IS THE ROAD''''' */
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &spi_bus, SPI_DMA_CH_AUTO));
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/


    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE TOUCHSCREEN↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    // instantiate the touchscreen device
    TouchScreen ts(touchHandle);
    ts.begin();
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/


    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE DISPLAY SCREEN↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    DisplayScreen ds(displayHandle);
    ds.begin();
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/

    /*↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓THE CANVAS↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*/
    Canvas cnv(&ds);
    /*↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑*/
    
    while(true) {
        cnv.render();
        if (ts.screenTouched())
        {
            printf("X: %u, Y: %u\n", ts.get_X(), ts.get_Y());
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}