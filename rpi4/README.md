# Raspberry Pi 4 Configuration
We use the raspberry pi 4 as a:
  - MQTT Broker
  - EdgeImpulse node for running the IA for detecting objects.

## Configure the raspberry pi
You can configure your raspberry pi liunx image with [rpi imager](https://github.com/raspberrypi/rpi-imager), which comes pretty handy since you can:

  - Enable wifi with an SSID and password.
  - Enable ssh and give it a public key that is allowed to connect to the rpi
  - Add a username and password

This saved us a lot of time, and we didn't have to use any ethernet cable or screen for configuring it.

## Install mosquitto broker
Install mosquitto with:

```bash
# We're using the raspbian image for the rpi
$ sudo apt-get update && sudo apt-get upgrade
$ sudo apt-get install mosquitto
```

## Run mosquitto broker
Once mosquitto is installed, copy our configuration file and run it with:

```bash
$ mosquitto -c mosquitto.conf -v
```

It will serve the mqtt server on port 1883.

## 
