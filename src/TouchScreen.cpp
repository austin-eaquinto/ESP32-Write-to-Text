#include "TouchScreen.h"

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

void IRAM_ATTR TouchScreen::irq_handler(void* arg)
{
    // casting the generic void pointer 'arg' to a TouchScreen pointer
    TouchScreen* ts = (TouchScreen*)arg;

    // use the pointer to call the logic handler
    ts->handle_touch();
}