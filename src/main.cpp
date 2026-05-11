#include "TouchScreen.h"

void app_main() {
    // ESP-IDF function to start the service that listens for interrupts
    gpio_install_isr_service(0);

    TouchScreen ts;

    ts.begin();
}