#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DisplayScreen.h"

// constructors
DisplayScreen::DisplayScreen(spi_device_handle_t handle, gpio_num_t csPin, gpio_num_t resetPin, gpio_num_t dcPin, gpio_num_t bckltPin)
    : _dispHandle(handle), D_CS_Pin(csPin), D_RESET_Pin(resetPin), D_DC_Pin(dcPin), BCKLT_Pin(bckltPin)
{
}

// methods
void DisplayScreen::begin()
{
    /* 1. Initialize the internal private variables in the constructor 
          initializer list.
       2. Fill out the spi_device_interface_config_t and call spi_bus_add_device.
       3. Use a gpio_config_t struct to set up Reset, DC, and Backlight all 
          as digital outputs.
    */

   // configure the display screen
    spi_device_interface_config_t display_devcfg = {};
    display_devcfg.clock_speed_hz = 15 * 1000 * 1000;
    display_devcfg.spics_io_num = D_CS_Pin;
    display_devcfg.queue_size = 7;

    // add display screen to the SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &display_devcfg, &_dispHandle));

    gpio_config_t display{};
    // bitwise OR each pin into 64-bit number to configure them all at once
    display.pin_bit_mask = (((1ULL << D_RESET_Pin) | 1ULL << D_DC_Pin) | 1ULL << BCKLT_Pin);
    display.mode = GPIO_MODE_OUTPUT;            // because it's a display
    display.pull_up_en = GPIO_PULLUP_DISABLE;   // output pins don't need pull-up/down resistors
    display.pull_down_en = GPIO_PULLDOWN_DISABLE;
    display.intr_type = GPIO_INTR_DISABLE;      // because outputs don't listen for incoming signals

    gpio_config(&display);  // submit the form to the hardware

    // initialization sequence to turn on the display
    gpio_set_level(D_RESET_Pin, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(D_RESET_Pin, 0);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(D_RESET_Pin, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    // light up the screen
    gpio_set_level(BCKLT_Pin, 1);

    // the struct blueprint of instructions to initialize the screen
    struct ScreenCycle {
        uint8_t cmd_byte;   // type of command. "the next data is a command"
        uint8_t length;     // how long the data for the command is
        uint8_t data[16];   // the list of commands to follow
        /* For me: the element just above this comment sets aside 128 bits for data. This 
            is fine because it is not accessible to a user (read only data) which means 
            there is no concern for a buffer overflow attack. */
    };

    // the actual struct instructions that will initialize the screen
    static const ScreenCycle init_sequence[] = {
        // { cmd_byte, length, { data bytes } }
        /*  1. Which setting to adjust.
            2. How many parameters follow the command.
                If length = 0, toggle switch
                If length >= 1, the command requires fine-tuning data
            3. The configuration values being put into the register. */
        { 0x11, 0, {} },        // command, no length, so no data
        { 0x3A, 1, {0x55} },    // command, length = 1 byte, data
        { 0x29, 0, {} }         // command, no length, so no data
    };

    /* Filing cabinet example */
    int cmd_num = sizeof(init_sequence) / sizeof(init_sequence[0]);

    // a folder in the filing cabinet
    for(int i = 0; i < cmd_num; i++)
    {
        sendCommand(init_sequence[i].cmd_byte);
        if(init_sequence[i].cmd_byte == 0x11)
        {
            vTaskDelay(pdMS_TO_TICKS(120));
        }
        // the file(s) in the folder in the filing cabinet
        /* The specific command to run is had at this point. This line
            is looking for the length (in bytes) of the command's data 
        */
        for (int j = 0; j < init_sequence[i].length; j++)
        {
            // the specific command to run from init_sequence
            // 
            init_sequence[i].data[j];
        }
    }
}

void DisplayScreen::sendCommand(uint8_t cmd)
{
    // tell ST7796S's display it's time to receive a command
    gpio_set_level(D_DC_Pin, 0); // low for command

    spi_transaction_t tx{};
    tx.length = 8; // number of bits
    tx.tx_buffer = &cmd; // address of the command byte

    // send the command to the hardware
    spi_device_polling_transmit(_dispHandle, &tx);
}

void DisplayScreen::sendData(uint8_t data)
{
    // tell ST7796S's display it's time to receive data
    gpio_set_level(D_DC_Pin, 1); // high for data

    spi_transaction_t tx{};
    tx.length = 8;
    tx.tx_buffer = &data;

    spi_device_polling_transmit(_dispHandle, &tx);
}
