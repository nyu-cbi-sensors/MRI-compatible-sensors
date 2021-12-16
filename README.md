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
![diagram of hardware for the wireless system](/assets/blockdiagram.png)
The Raspberry Pi acts as a center to collect and save data from all of the different wireless sensor nodes. Each of the feathers can be connected to a number of different sensors, limited by whatever protocol the sensor needs in order to work, whether that be GPIO pins with SPI, or intersections with I2C, or clock speed depending on your desired sampling rate.
## Raspberry Pi Overview
There's a few modules that are needed on the Pi to function. Below are instructions on how to install the modules, and the code for the NodeRED flow is in the repository.
### RaspAP
To act as an independent wireless hub for the sensors, the Raspberry Pi can act as a standalone Wireless Access Point (WAP). This means it acts like a Wi-Fi router, letting multiple devices connect to its network and share information with each other. There are several ways to do this, I have used [RaspAP](https://raspap.com/) to get started. I would highly recommend to change the default password for the RaspAP module.
### Mosquitto
To organize the communication of information between the Raspberry Pi and the sensors, the Raspberry Pi ran as an MQTT broker using Mosquitto. There are several tutorials on how to set this up already, I have used the one from [randomnerdtutorials](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/). I used the default settings.
### NodeRED
The last thing needed is a way to interface with the devices in order to see what is connected and to save information. Again, several ways to do this, but I ended up using [NodeRED](https://nodered.org/docs/getting-started/raspberrypi) to take care of a lot of the heavy lifting for me. It also allow for custom scripting with JavaScript when the provided nodes do not suit your needs. You need to also install the dashboard community module to use the flow I have provided [here](/flow_twoesp32bno08x.json) to be an interface to collecting data from the sensors.

## Sensor node overview
### Hardware
![fritzing diagram of sensor node](/assets/fritzing_schematic_sensor.png)
A nickel free Li-ion battery (from Powerstream) is used to power an Adafruit ESP32 Feather which is connected to an Adafrut BNO08x breakout board over I2C. Four wires are needed between the Feather and BNO08x (VDD, GND, SCL, SDA). I modified the default Adafruit ESP32 Feather to have an u.FL version of the ESP32 module. This is to allow for use of an external u.FL Wi-Fi antenna, to allow the rest of the sensor node be encapsulated by a metal box to prevent MRI interference.
### Software
I would recommend using the PlatformIO (I use it as an extension of Visual Studio Code) to manage dependencies automatically. The main.cpp file should be able to be used as a base to be able to compiile and upload using the Arduino IDE.
The following libraries are needed, and defined in the platform.ini file.
- knolleary/PubSubClient@^2.8
- bblanchon/ArduinoJson@^6.17.3
- adafruit/Adafruit BNO08x @ ^1.2.1

The PubSub client is used to talk to the Raspberry Pi's MQTT broker. The Arduino JSON library is used to tidily pack up the sensor information before transferring the information over MQTT. The Adafruit library is used to talk with the BNO08x IMU, accessing the different sort of information that the IMU provides, such as linear acceleration, raw acceleration, or orientation.

The Visual Studio Code version I used is 1.63.1.
The PlatformIO version I used is Core 5.2.3 and Home 3.4.0.


# Component Selection
Have a magnet on hand to test the magnetic properties of components before bringing close to the MRI machine. If using a small enough magnet, you can isolate which component needs to get removed/replaced and find an appropriate replacement, such as the USB connector or battery. Not all parts will be able to be replaced. The Adafruit ATWINC1500 board could not be used in a 3T magnet due to the magnetic properties of the shield and components in the wireless module. Capacitors and USB connectors are commonly magnetic, but usually have non-magnetic components also available to order, but not as common.

# Security
There are probably several security holes in this setup. At the very least I would change the default password for the Raspberry Pi and the access point, and the administrative password for RaspAP. 
