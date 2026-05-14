#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "driver/gpio.h"       // espidf specific (not Arduino). for gpio_num_t
#include "driver/spi_master.h" // spi bus handling

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
  static volatile bool isTouched;

  /*  - IRAM_ATTR - ESP32 attribute that places the function in internal RAM to
        execute faster. Good for interrupts.
      - type void* arg is a generic pointer to memory and the placeholder
        argument that irq_handler will use to put the 'this' pointer in that
        location.
      - when irq_handler is called the argument used is the 'this' pointer.
        This way, C++'s 'this' member is placed in memory like in C.
      - it runs then is let go because of the void type.
  */
  static void IRAM_ATTR irq_handler(void *arg);
  void handle_touch(); // handles the logic

public:
  // constructors
  TouchScreen();
  TouchScreen(spi_device_handle_t _spiHandle, gpio_num_t irqPin, gpio_num_t csPin);

  // getters
  static bool getIsTouched() { return isTouched; }
  static bool setIsTouched(bool value) { isTouched = value; }

  // methods
  void begin();
};

#endif