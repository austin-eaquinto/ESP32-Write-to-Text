# Write-to-Text
![Status](https://img.shields.io/badge/status-in%20development-orange) ![Platform](https://img.shields.io/badge/platform-ESP32-blue) ![Display](https://img.shields.io/badge/display-ST7796S-purple) ![License](https://img.shields.io/badge/license-MIT-green)

## Overview
Write-to-Text is an embedded firmware project for the ESP32 that enables handwriting recognition
directly on a touchscreen display. Using the touch input capabilities of the ST7796S display,
users can write individual letters freehand, which are then interpreted and converted into digital text.
> Currently in active development. The TouchScreen driver class and touch interrupt
> handler are being implemented. Contributions and feedback are welcome.

## Hardware Requirements
**Microcontroller:** ESP32 DevKit V1
**Display/Touch:** ST7796S TFT Touchscreen
**Additional Components:** 1x 10kΩ pull-up resistor (T_IRQ line to 3.3V)
**Power:** 5V via VIN pin
### Pin Connections

| ST7796S Pin | ESP32 GPIO | Description |
|-------------|------------|-------------|
| **Touch Controller** | | |
| T_IRQ | D34 (GPIO 34) | Touch interrupt — input only pin, 10kΩ pull-up to 3.3V |
| T_DO | D19 | Touch SPI MISO |
| T_DIN | D23 | Touch SPI MOSI |
| T_CLK | D18 | Touch SPI clock |
| T_CS | D25 | Touch chip select |
| **Display** | | |
| SDO (MISO) | D19 | Display SPI MISO — shared with T_DO |
| SDI (MOSI) | D23 | Display SPI MOSI — shared with T_DIN |
| SCK | D18 | Display SPI clock — shared with T_CLK |
| CS | D5 | Display chip select |
| DC/RS | D26 | Data/command select |
| RESET | D4 | Display reset |
| LED | D22 | Backlight — GPIO for PWM dimming or 3.3V for constant on |
| **Power** | | |
| VCC | VIN | 5V power |
| GND | GND | Ground |