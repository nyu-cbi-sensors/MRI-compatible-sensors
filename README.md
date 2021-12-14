# MRI-compatible-sensors
This is a collection of work done to create a flexible MRI compatible wireless sensor system using off the shelf components. This example shows how to simultaneously collect information from two different sensors located inside of an MRI bore. 

# Materials needed
- Raspberry Pi 4
- [Adafruit ESP32 Feather board](https://www.adafruit.com/product/3405) (x2)
- [ESP32 U.FL module](https://www.mouser.com/ProductDetail/Espressif-Systems/ESP32-WROOM-32UM113DH3200UH3Q0?qs=W%2FMpXkg%252BdQ4Fqx%2FReRQpFQ%3D%3D&mgh=1&gclid=Cj0KCQiAnuGNBhCPARIsACbnLzoCWG-Ctl-NIyBCex6A-nVdB5Kj8ZWKIZyTE6AEg8IRo4yzbLntOfIaApvoEALw_wcB)
- [u.fl PCB antenna](https://www.digikey.com/en/products/detail/pulselarsen-antennas/W3921B0100/7667486)
- [Sparkfun ADXL313 Accelerometer breakout board](https://www.sparkfun.com/products/17241)
- [Adafruit BNO08x IMU breakout board](https://www.adafruit.com/product/4754)
- [Nickel-free Lithium-ion battery](https://www.powerstream.com/non-magnetic-lipo.htm)
- hookup wire
- solder
- copper tape/metal box

# Setup
## Overview
The Raspberry Pi acts as a center to collect and save data from all of the different wireless sensor nodes. Each of the feathers can be connected to a number of different sensors, limited by whatever protocol the sensor needs in order to work, whether that be GPIO pins with SPI, or intersections with I2C, or clock speed depending on your desired sampling rate.
## Setting up the Raspberry Pi
There's a few modules that are needed on the Pi.
### RaspAP
To act as an independent wireless hub for the sensors, the Raspberry Pi can act as a standalone Wireless Access Point (WAP). This means it acts like a Wi-Fi router, letting multiple devices connect to its network and share information with each other. There are several ways to do this, I have used [RaspAP](https://raspap.com/) to get started.
### Mosquitto
To organize the communication of information between the Raspberry Pi and the sensors, the Raspberry Pi ran as an MQTT broker using Mosquitto. There are several tutorials on how to set this up already, I have used the one from [randomnerdtutorials](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/)
### NodeRED
The last thing needed is a way to interface with the devices in order to see what is connected and to save information. Again, several ways to do this, but I ended up using [NodeRED](https://nodered.org/docs/getting-started/raspberrypi) to take care of a lot of the heavy lifting for me. It also allow for custom scripting with JavaScript when the provided nodes do not suit your needs.

## Setting up the sensor nodes
### Hardware
(put in frizting diagrams here)
### Software
The code provided can be uploaded to the boards with the Arduino IDE. I would recommend using the PlatformIO extension of Visual Studio Code to manage dependencies, and the code for that is also provided.
### add version numbers/libary numbers

# Component Selection
Have a magnet on hand to test the magnetic properties of components before bringing close to the MRI machine. If using a small enough magnet, you can isolate which component needs to get removed/replaced and find an appropriate replacement, such as the USB connector or battery. Not all parts will be able to be replaced. The Adafruit ATWINC1500 board could not be used in a 3T magnet due to the magnetic properties of the shield and components in the wireless module. Capacitors and USB connectors are commonly magnetic, but usually have non-magnetic components also available to order, but not as common.

# Security
There are probably several security holes in this setup. At the very least I would change the default password for the Raspberry Pi and the access point, and the administrative password for RaspAP. 
