# Hackwinkel badge

This repository contains the software and data files for the hackwinkel 2020 badge.

Animated gif movie of the badge in action: http://hackwinkel.nl/badge_small.gif

TL;DR usage:

- Download and install the Arduino IDE
- Add the espressif ESP32 board manager URL to the Arduino IDE https://dl.espressif.com/dl/package_esp32_index.json
- Add the ESP32 boards to the board manaer
- Select the ESP32 Wrover module as the target board
- Select the "No OTA (1MB APP/3MB SPIFFS)" partition scheme
- Install the Adafruit ST7789 library (plus dependencies) through the Arduino IDE
- Install the Tjpg JPEG decoder library throuh the Arduino IDE
- Get the "AsyncTCP" library ZIP: https://github.com/me-no-dev/AsyncTCP/archive/master.zip
- Get the "ESPAsyncWebServer" library ZIP: https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip
- Install both ZIP libraries through the Arduino IDE
- Download this ("badge") software repository and unpack it to your Arduino sketches folder
- Open the sketch, connect the FTDI programmer and click "upload"
- On the programmer click and hold the GP0 button, click and release the reset button, then release the GP0 button
- Wait for the upload to finish

The sketch should now be uploaded to the badge. Reset the badge and enjoy.

To push the data files included in the data directory onto the badge:

- Install the ESP8266 filesystem uploader as per instructions: https://github.com/esp8266/arduino-esp8266fs-plugin
- After installation restart the IDE
- Connect the badge to the FTDI programmer and click Tools -> Data upload
- On the programmer click and hold the GP0 button, click and release the reset button, then release the GP0 button
- Wait for the upload to finish

The data should now be uploaded to the badge. Reset the badge and enjoy.

In some cases the upload of program or data may fail because a needed python library is not installed. If this happens install the python-serial library using "sudo apt-get install python-serial" or any other means appropriate for your OS.
In some cases you may temporarily not have access to the 'dialout' group after first installing the Arduino IDE. This prevents you from uploading data/programs. The simplest way to resolve this is by rebooting your computer.


