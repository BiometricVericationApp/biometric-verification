## Activity Monitor

## Flashing the firmware:

   - Run it with platformio (recommended)
   - Install the dependencies yourself and build it with arduino-ide.


### Flashing with platformio
We love platformio because so you can build the image with:

```bash
$ sudo pio run
```

And write it to the esp32 with:
```bash
$ sudo pio run -t upload
```


### Arduino IDE
We added symbolic links on this root folder just to mantain compatibility with arduino-ide (because it is easier for noobies).

See the platformio.ini file, and see which dependencies do you have to install.

Then build and deploy as you need
