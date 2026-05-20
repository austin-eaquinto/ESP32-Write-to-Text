#include "TouchScreen.h"
#include <cstring>>

// constructors
// TouchScreen::TouchScreen();
TouchScreen::TouchScreen(spi_device_handle_t handle, gpio_num_t irqPin, gpio_num_t csPin)
    : _spiHandle(handle), T_IRQ_Pin(irqPin), T_CS_Pin(csPin)
{
}

// methods
void TouchScreen::begin()
{
    /* ai's helpful advice for structs:
       - "Think of the gpio_config_t struct as a literal paper form you are 
          filling out for the ESP32’s hardware department." It's just paper/form. 
          I'm just filling out the sections. */
    gpio_config_t touch;
    /*  1ULL means the number 1 as an 'unsigned long long' integer.
        It can't be negative and takes up 64 bits.
        Type of uint64_t to handle the 64 bits. */
    touch.pin_bit_mask = (1ULL << T_IRQ_Pin);
    touch.mode = GPIO_MODE_INPUT;               // set as an input
    touch.pull_up_en = GPIO_PULLUP_ENABLE;      // set resistors
    touch.pull_down_en = GPIO_PULLDOWN_DISABLE; // 
    touch.intr_type = GPIO_INTR_NEGEDGE;        // the interrupt trigger
    gpio_config(&touch); // submit the filled out paper (instructions) to hardware

    // ESP-IDF function to start the service that listens for interrupts
    // gpio_install_isr_service(0);
    // ESP-IDF function to connect a specific pin to my irq_handler
    // needs 3 args. the pin number, function address and the argument to pass to this function/address of current object
    gpio_isr_handler_add(T_IRQ_Pin, irq_handler, this);
}

// SPI is full duplex
// setting up and using the XPT2046 chip that handles touch on the screen
void TouchScreen::handle_touch()
{
    uint8_t X = 0x90;   // the X coordinate??
    uint8_t tx_buf[3] = {X, 0x00, 0x00};    // sends the command, keeps clk running to listen for answer
    uint8_t rx_buf[3] = {0x00,0x00,0x00};   // receiver. bits that leave tx_buf enter here

    /* Recreated each time this function is called. Standard way to do it.
        Created locally on the stack.*/
    spi_transaction_t t;        // initialize the struct
    // used here to zero out memory
    // args are: starting address of t, the value to be set, and # of bytes to fill
    memset(&t, 0, sizeof(t));
    /* set the properties of t (fill out the paper form) */
    t.length = 24;  // how many bits to clock out over the wire during transmission. 8 bits to send out, 16 to recieve 12
    t.tx_buffer = tx_buf;   // points master device to the command byte
    t.rx_buffer = rx_buf;    // doesn't need '&' bc/array naturally points to its start

    if (gpio_get_level(T_IRQ_Pin) == 0) // check the state of the interrupt pin
    {
        gpio_set_level(T_CS_Pin, 0);    // set the state of the Chip Select pin
    }

    // transmit the struct 't' over the SPI bus
    // args: (configured SPI device handle, struct pointer)
    // with this size of data (24 bits) polling is more efficient here
    spi_device_polling_transmit(_spiHandle, &t);
}

// ISR that runs if the screen is touched
void IRAM_ATTR TouchScreen::irq_handler(void* arg)
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
    TouchScreen* ts = (TouchScreen*)arg;

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
bool TouchScreen::screenTouched()
{
    if(_touchTriggered)
    {
        handle_touch();     // handle the logic of the touchscreen
        _touchTriggered = false;
        return true;
    }
    return false;
}