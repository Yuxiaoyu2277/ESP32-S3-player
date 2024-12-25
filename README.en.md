# Time Tape (ESP32 S3 MP3 Player)
# [中文版](README.md) / [English Version](README.en.md)

## Overview
This project provides a hardware and software guide to build an MP3 player based on the ESP32 S3 microcontroller. The device features a retro cassette-like case with functionalities for music playback and time display.

## Features
- ESP32 S3 Microcontroller: The core of the MP3 player, powered by Espressif Systems' MCU chip with integrated 2.4 GHz Wi-Fi and Bluetooth 5 (LE), running at a frequency of up to 240 MHz.
- 1.14-inch TFT LCD Screen: Provides a user interface for the player.
- RTC Chip (BM8563): Ensures accurate offline time display after initial calibration.
- WM8978 DAC: Offers independent audio output, integrates a stereo differential microphone preamplifier, and includes drivers for speakers, headphones, and differential or stereo line outputs.
- NS4150B Power Amplifier: Enhances audio output with ultra-low EMI and a 3W mono Class-D amplifier requiring no filter.
- ETA6002 Charging Chip: A lithium battery charging chip with up to 2A charging current, path management, and independent power supply for the system.
- USB Type-C Port: Facilitates charging and data transfer.
- SD/TF Card Reader: Allows storage expansion, supporting up to 32GB (limited by ESP32 S3 compatibility).
- Headphone Jack: Standard 3.5mm jack for connecting headphones.
- Speaker: Uses an ultra-thin speaker salvaged from phones, optimized for the best sound quality.

## Hardware Components
- ESP32 S3: Main processing unit with integrated Wi-Fi and Bluetooth.
- TFT LCD (1.14-inch): Display module for user interface.
- BM8563 RTC: Real-time clock chip for offline timekeeping.
- WM8978 DAC: Audio converter chip for high-fidelity sound.
- NS4150B PA: Power amplifier chip for audio output.
- ETA6002 Charger: Lithium battery charging chip.
- USB Type-C Port: For charging and data transfer.
- SD/TF Card Reader: For storage expansion.
- Headphone Jack: 3.5mm audio output.
- Speaker: Provides audio playback.
- Buttons: Five buttons for UI interaction and control.
- Switch: One switch for power on/off control.

## Circuit Diagram

## Getting Started
1. Gather Components: Collect all the necessary components listed above.
2. Assemble the Circuit: Assemble the circuit on a breadboard or PCB following the circuit diagram.
3. Program the ESP32 S3: Use the Arduino IDE or any compatible platform to write and upload firmware to the ESP32 S3.
4. Test Functionality: Power the device and test all functionalities, including audio playback, screen display, and timekeeping.

## Firmware
- The firmware for this project is open source and can be found in the main branch of this repository. It includes functionalities such as audio playback, screen management, and RTC features.

## Code

## Contribution
- Contributions are welcome! If you find any issues or have suggestions for improvement, please open an issue or submit a pull request.

## License
- This project is licensed under the GNU General Public License. You are free to use, modify, and distribute the code as long as you comply with the terms of the license. Attribution is mandatory! You must include the GitHub repository link or credit the author: 一条优秀的闲鱼呀！

## Contact
- For any questions or inquiries, please contact the maintainer via QQ: 2277236944 or Email: Oliver.yuchunto@gmail.com
