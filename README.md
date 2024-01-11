# Biometric Verification

## Tree structure of the project:
The structure of the project looks like this:
```
.
├── activity-monitor
├── common
├── docs
├── linux-router
├── presence
├── rpi4
└── user-data-collector
```
Where we can distinct between the folders that contain code for the embedded systems, which are:

   - activity-monitor: All the code related with the activity monitor (screen and leds).
   - presence: All the code related with the ultrasound stations
   - user-data-collector: All the code related with the Galvanic and Hearth Rate sensor.


The folders that contains config files / code for non embedded systems (such as a pc, rasberry pi, etc):

   - rpi4: Contains the configuration file for the broker and the neural network.
   - linux-router: Contains the configuration file and instructions for run your ap on your 


And the remaining folders are for github, documentation, etc.

## Dependencies:
You will need [platformio](https://platformio.org/) for building all the firmwares. On archlinux install it with:

```
$ sudo pacman -S platformio-core platformio-udev
```

## Flashing the firmware:
For flashing the firmware of the embedded systems you have two options:

   - Run it with platformio (recommended)
   - Install the dependencies yourself and build it with arduino-ide.


### Flashing with platformio
We love platformio because so you can build the image with:
```bash
$ sudo pio run
```

And write it to the device with:

```bash
$ sudo pio run -t upload
```

### Arduino IDE
We added symbolic links on this root folder just to mantain compatibility with arduino-ide (because it is easier for noobies).

See the platformio.ini file, see which dependencies do you have to install, and install them manually.

Then build and deploy as you need.
