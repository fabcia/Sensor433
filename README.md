# Sensor433
[![Build Status](https://travis-ci.org/LarsBergqvist/Sensor433.svg?branch=master)](https://travis-ci.org/LarsBergqvist/Sensor433)

An Arduino Library for sending and receiving values between Arduino boards with simple 433MHz transmitters and receivers
A typical use case is to let small battery powered Arduino Pro Mini boards send sensor values to one single ESP8266 board that creates MQTT messages on the WiFi network from the values.
<p>A sensor value is encoded with a sensor id and checksum into a 4-byte message that is transmitted over 433MHz.
<p>The message has this structure (from sx to dx)
<p>6 bit sensorId (0 to 63)
<p>2 bit typeSensor (0 to 3)
<p>12 bit sensordata float between -20.47 and + 20.47 or an unsigned int, i.e. between 0 and 4047
<p>8 bit checksum
<p>See the included examples for transmitting, receiving and bridging to MQTT messages.
<p>This library depends on the rc-switch library: https://github.com/sui77/rc-switch


