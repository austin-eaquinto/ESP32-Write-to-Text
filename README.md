# Write-to-Text
![Status](https://img.shields.io/badge/status-in%20development-orange) ![Platform](https://img.shields.io/badge/platform-ESP32-blue) ![Display](https://img.shields.io/badge/display-ST7796S-purple) ![License](https://img.shields.io/badge/license-MIT-green)

## Overview
Write-to-Text is an embedded firmware project for the ESP32 that enables handwriting recognition
directly on a touchscreen display. Using the touch input capabilities of the ST7796S display,
users can write individual letters freehand, which are then interpreted and converted into digital text.
> Currently in active development. The TouchScreen driver class and touch interrupt
> handler are being implemented. Contributions and feedback are welcome.

## Hardware Requirements
**Microcontroller:** ESP32-S3 DevKit C-1 n8r8<br>
**Display/Touch:** ST7796S TFT Touchscreen<br>
**Additional Components:** 1x 10kΩ pull-up resistor (T_IRQ line to 3.3V)<br>
**Power:** 5V via VIN pin
### Pin Connections

| ST7796S Pin | ESP32 GPIO | Description |
|-------------|------------|-------------|
| **Touch Controller** | | |
| T_IRQ | 36 | Touch interrupt — input only pin, 10kΩ pull-up to 3.3V |
| T_DO | 11 | Touch SPI MISO |
| T_DIN | 13 | Touch SPI MOSI |
| T_CLK | 12 | Touch SPI clock |
| T_CS | 10 | Touch chip select |
| **Display** | | |
| SDO (MISO) | 11 | Display SPI MISO — shared with T_DO |
| SDI (MOSI) | 13 | Display SPI MOSI — shared with T_DIN |
| SCK | 12 | Display SPI clock — shared with T_CLK |
| CS | 5 | Display chip select |
| DC/RS | 17 | Data/command select |
| RESET | 4 | Display reset |
| LED | 2 | Backlight — GPIO for PWM dimming or 3.3V for constant on |
| **Power** | | |
| VCC | VIN | 5V power |
| GND | GND | Ground |

## Classes (WIP)
| Name | Purpose |
|-------------|------------|
| main.cpp | Call Classes & run the program |
| TouchScreen | Handles the math and data logging for coordinates |
| DisplayScreen | Creates an address window for the screen to correctly display data within set bounds |