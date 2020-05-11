# Cumulocity Example for ESP Boards with device registration (Bootstrap process)
This is an example for an ESP Board in order to communicate RSSI and other Data to the cumulocity platform via MQTT. Its an short example on how to implement the basic communication on a very small device or how to implement communication embedded.
Cumulocity is an IoT platform that enables rapid connections of many, many different devices and applications. It allows you to monitor and respond to IoT data in real time and to spin up this capability in minutes. More information on Cumulocity IoT and how to start a free trial can be found [here](https://www.softwareag.cloud/site/product/cumulocity-iot.html#/).

Cumulocity IoT enables companies to to quickly and easily implement smart IoT solutions.

These tools are provided as-is and without warranty or support. They do not constitute part of the Software AG product suite. Users are free to use, fork and modify them, subject to the license agreement. While Software AG welcomes contributions, we cannot guarantee to include every contribution in the master project.

Contact us at [TECHcommunity](mailto:technologycommunity@softwareag.com?subject=Github/SoftwareAG) if you have any questions.

## ESP 8266 / ESP 32

The ESP8266 is a low-cost Wi-Fi microchip with full TCP/IP stack and microcontroller.
The ESP-01 module allows microcontrollers to connect to a Wi-Fi network and make simple TCP/IP connections using Hayes-style commands. Since it is very cheap and has multiple in- and outputs it is very common in IoT Projects or PoC´s.

## Preparation of Arduino IDE

The Arduino IDE is very easy to use.
In order to setup your Arduino IDE to work with your esp8266/esp32 arduino compatible module you need to make the following steps:

    1. Connect your ESP8266 or ESP32 Module to PC
    2. Open your Arduino IDE
    3. Go to File -> Preferences
    4. Add this link to Additional Board Manager
    5. Go to Tools -> Board Manager
    6. Find ESP8266/ESP32 board set and activate it
    7. Select matching entry for your board within Tools->Boards
    8. Choose your programmer COM port

## Device Creation

The Device will make use of the bootstrap process within cumulocity.
So there is no need to create some credentials just for this device.
Instead you can register the device with the devices MAC Address using the default device registration process.

To make that work, you have to provide the correct URL and Bootstrap credentials for your tenant/master tenant.
In case you don't have those, please get in contact with your tenant administrator.

Additional Information regarding device creation can be found in the official documentation.

 1. [Cumulocity Documentation](https://cumulocity.com/guides/device-sdk/introduction/)

## Example Code

Within the loop() function the RSSI of the WLAN module will be send in a 30s interval.
Code is already prepared to also work with an optional BME280 temperature and humidity sensor.
You can find additonal information regarding the MQTT interface on the MQTT cheat sheet or in the documentation.

 1. [Cheat Sheet](https://support.cumulocity.com/hc/en-us/article_attachments/360000089547/cheatsheet.pdf)
 2. [MQTT Interface](https://cumulocity.com/guides/device-sdk/mqtt-examples/)
