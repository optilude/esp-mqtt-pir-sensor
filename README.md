# ESP MQTT PIR Sensor

Forked from <https://github.com/bruhautomation/ESP-MQTT-JSON-Multisensor>.

This version uses a NodeMCU 1.0 board and an AMT312 PIR sensor only.

It publishes a message on an MQTT topic of either `on` or `off` when motion
is detected/no longer detected.

Original video tutorial - <https://youtu.be/jpjfVc-9IrQ>

## Building

You need to use the Arduino IDE to build `motion_sensor.ino` from source. Before
you can do so, you must copy `secrets.h.template` to a new file called
`secrets.h` and edit this file to specify your wifi network and MQTT server. If
you want, you can also change the name of the MQTT topic and the host name and
password for OTA (over-the-air) updates (see below) in this file.

You will also need to add the following dependencies in the Arduino IDE:
  
- Support for the ESP8266 boards:
  - You can add it to the board manager by going to File -> Preference and
    pasting `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
    into the Additional Board Managers URL field.
  - Next, download the ESP8266 dependencies by going to Tools -> Board ->
    Board Manager and searching for ESP8266 and installing it.

- You will also need to download the follow libraries by going to
  Sketch -> Include Libraries -> Manage Libraries
  - Adafruit unified sensor
  - PubSubClient

## OTA Uploading

This code also supports remote uploading to the ESP8266 using Arduino's OTA
library. To utilize this, you'll need to first upload the sketch using the
traditional USB method. However, if you need to update your code after that,
your WIFI-connected ESP chip should show up as an option under Tools -> Port ->
Sensor Name at your.ip.address.xxx. More information on OTA uploading can be
found [here](http://esp8266.github.io/Arduino/versions/2.0.0/doc/ota_updates/ota_updates.html).
Note: You cannot access the serial monitor over WIFI at this point.  

## Wiring

The PIR sensor is assumed to be connected to pin `D5`. You also need to run
power (3.3V and ground) to the sensor, and separately power the NodeMCU.
