#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TouchScreen.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include <cstring>

// constructors
// TouchScreen::TouchScreen();
TouchScreen::TouchScreen(spi_device_handle_t handle, gpio_num_t irqPin, gpio_num_t csPin)
    : _spiHandle(handle), T_IRQ_Pin(irqPin), T_CS_Pin(csPin)
{
}

// methods
void TouchScreen::begin()
{
    // configure the touchscreen ⌄⌄⌄
    spi_device_interface_config_t touch_devcfg = {};
    touch_devcfg.clock_speed_hz = 1 * 1000 * 1000;
    touch_devcfg.spics_io_num = T_CS_Pin;
    //
    touch_devcfg.queue_size = 7;

    // add touchscreen to the SPI bus
    // 3 args- Host, Address of configured device, Adress of the handle variable
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &touch_devcfg, &_spiHandle));

    /* ai's helpful advice for structs:
       - "Think of the gpio_config_t struct as a literal paper form you are
          filling out for the ESP32’s hardware department." It's just paper/form.
          I'm just filling out the sections. */
    gpio_config_t touch{};  // here {} sets all elements of the struct to zero's. This prevents garbage data in unused elements.
    /*  1ULL means the number 1 as an 'unsigned long long' integer.
        It can't be negative and takes up 64 bits.
        Type of uint64_t to handle the 64 bits. */
    touch.pin_bit_mask = (1ULL << T_IRQ_Pin);
    touch.mode = GPIO_MODE_INPUT;               // set as an input
    touch.pull_up_en = GPIO_PULLUP_ENABLE;      // set resistors
    touch.pull_down_en = GPIO_PULLDOWN_DISABLE;
    touch.intr_type = GPIO_INTR_NEGEDGE;        // the interrupt trigger. Runs only if volate is 0.0 (actual touch value)
    gpio_config(&touch);                        // submit the filled out paper (instructions) to hardware


    gpio_set_intr_type(T_IRQ_Pin, GPIO_INTR_NEGEDGE);

    // ESP-IDF function to start the service that listens for interrupts
    // gpio_install_isr_service(0);
    // ESP-IDF function to connect a specific pin to my irq_handler
    // needs 3 args. the pin number, function address and the argument to pass to this function/address of current object
    gpio_isr_handler_add(T_IRQ_Pin, irq_handler, this);
}

// SPI is full duplex
// setting up and using the XPT2046 chip that handles touch on the screen
// get the start X and Y coordinates on the screen where the user touches
/* Focused purely on reading the SPI bus. */
void TouchScreen::handle_touch()
{
    /* Recreated each time this function is called. Standard way to do it.
        Created locally on the stack.*/
    uint8_t X = 0x90;                       // the X coordinate
    uint8_t tx_buf[3] = {X, 0x00, 0x00};    // sends the command, keeps clk running to listen for answer
    uint8_t rx_buf[3] = {0x00, 0x00, 0x00}; // receiver. bits that leave tx_buf enter here
    spi_transaction_t tx; // initialize the struct

    // used here to zero out memory
    // args are: starting address of t, the value to be set, and # of bytes to fill
    memset(&tx, 0, sizeof(tx));
    /* set the properties of t (fill out the paper form) */
    tx.length = 24;        // how many bits to clock out over the wire during transmission. 8 bits to send out, 16 to recieve 12
    tx.tx_buffer = tx_buf; // points master device to the command byte
    tx.rx_buffer = rx_buf; // doesn't need '&' bc/array naturally points to its start

    // start listening for X coordinates
    // gpio_set_level(T_CS_Pin, 0); // set the state of the Chip Select pin. "Pick me!"

    // transmit the struct 't' over the SPI bus
    // args: (configured SPI device handle, struct pointer)
    // with this size of data (24 bits) polling is more efficient here
    spi_device_polling_transmit(_spiHandle, &tx); // transmit X coordinates
    // reset the CS pin to release the microprocessor from listening
    // gpio_set_level(T_CS_Pin, 1);

    // printf("RAW RX BUFFERS -> [0]: 0x%02X, [1]: 0x%02X, [2]: 0x%02X\n", rx_buf[0], rx_buf[1], rx_buf[2]);

    /* BELOW HERE--Logic handled by the microprocessor */
    // shift the bits
    /* 1. When the data is sent it is initially in the buffer following state (d = data)
        - data received = 0000 0000 0ddd dddd dddd d000
       2. The bits need to be moved to their correct places. But because this is working
          with array[3] and each element in the array is 8-bits, it looks like this
        - rx_buf[1] = 0,b11,b10,b9, b8,b7,b6,b5
        - rx_buf[2] = b4,b3,b2,b1, b0,0,0,0
        3. This part of the system needs 12 bits of data. To make this into useable data
           do the following code. These put each bit in the correct locations for the
           bitwise OR operation */

    // put bits 11-5 into locations 11-5
    uint16_t upper_data_bits = (uint16_t)rx_buf[1] << 4; // using uint16_t because I don't want to lose bits to the shift op
    // put bits 4-0 into locations 4-0
    uint16_t lower_data_bits = rx_buf[2] >> 4; // uint16_t instead of uint8_t so that C++ doesn't have to do extra work behind the scenes to convert for OR op
    // bitwise OR together to get final data value. Using uint16_t because the data is 12 bits total
    _rawX = upper_data_bits | lower_data_bits; // store X coordinates in .h private variable

    /* BELOW HERE--Y coordinate stuff */
    // create the variables + buffers
    uint8_t Y = 0xD0;                       // the Y coordinate
    uint8_t ty_buf[3] = {Y, 0x00, 0x00};
    uint8_t ry_buf[3] = {0x00, 0x00, 0x00};
    spi_transaction_t ty;

    // get the size of
    memset(&ty, 0, sizeof(ty));

    ty.length = 24;
    ty.tx_buffer = ty_buf;
    ty.rx_buffer = ry_buf;

    // gpio_set_level(T_CS_Pin, 0);

    spi_device_polling_transmit(_spiHandle, &ty);

    // gpio_set_level(T_CS_Pin, 1);

    uint16_t y_upper = (uint16_t)ry_buf[1] << 4;
    uint16_t y_lower = ry_buf[2] >> 4;
    _rawY = y_upper | y_lower; // store Y coordinates in .h private variable

    /* Calculate the raw X and Y data for immediate use in main.cpp
        1.  
        width = 320
        height = 480
         */
}

// ISR that runs if the screen is touched
IRAM_ATTR void TouchScreen::irq_handler(void *arg)
{
    // casting the generic void pointer 'arg' to a TouchScreen pointer
    /*  ...Explicit Type Cast...
        - "Take this variable arg and treat it as a pointer to an object of type
        TouchScreen."
        - TouchScreen: the name of the Class or struct
        - * indicates a pointer type
        - (TouchScreen*) is the cast operator that forces the conversion
        - arg: the variable being converted which is usually a pointer of a
        different type, like void* (see irq_handler's argument type). */
    TouchScreen *ts = (TouchScreen *)arg;

    gpio_intr_disable(ts->T_IRQ_Pin);   // temporarily disable interrups on the pin so it doesn't lock up the CPU

    /* I left this commented out code here to help me remember what I learned
        about SPI communication speed. the IRAM_ATTR type is really fast and
        comparatively SPI communication is very slow. With the code here the
        interrupt would have happened but then had to wait for the SPI to finish
        communicating before it could finish. This could cause big delays because
        it freezes the CPU until complete. handle_touch was moved into main and
        a static flag isTouched was created in TouchScreen.h.

        A lot of functions used for SPI are blocking, which causes crashes in an
        ISR.

        Process: Screen is touched->irq_handler runs and sets flag to true->flag
        is read in main.cpp in an if statement->if flag is true, ISR runs and
        the flag is set to false */
    // use the pointer to call the logic handler
    // ts->handle_touch();

    // the Flag used to tell the CPU there is an interrupt/ the screen was touched
    ts->_touchTriggered = true;
}

// Checks if the screen was touched and runs logic if true
/* Manages the high-level state machine and timing. See handle_touch() for SPI. */
bool TouchScreen::screenTouched()
{
    // tracking last screen touch
    static int64_t lastTouchTime = 0;
    int64_t currentTime = esp_timer_get_time();

    if (_touchTriggered)
    {
            /* DEBOUNCE BLOCK */
        // if the screen was touched less than 150ms ago, ignore the current touch
        if (currentTime - lastTouchTime < 150000)
        {
            _touchTriggered = false;
            gpio_intr_enable(T_IRQ_Pin);   // re-enable pin if it was a ghost bounce
            return false;
        }

        handle_touch(); // read SPI bus for coordinate data of screen touch

        // filter out touches that are too light
        if (_rawX == 2047 && _rawY == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            handle_touch();
        }

        // wait for user to stop touching, which will end the interrupt signal
        while (gpio_get_level(T_IRQ_Pin) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        // update timestamp of last successful read
        lastTouchTime = esp_timer_get_time();
        _touchTriggered = false;
        gpio_intr_enable(T_IRQ_Pin);   // clear residual triggers, re-enable interrupt for next press
        
        if (_rawX == 2047 && _rawY == 0)
        {
            return false;
        }
        
        return true;
    }
    return false;
}