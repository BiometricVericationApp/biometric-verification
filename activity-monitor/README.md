## Activity Monitor

### Configure it!
You can configure the parameters, such as:

  - [Miliseconds for re-check the wifi connection](hardware/wifi.h)
  - [Miliseconds for re-trying the wifi connection when it failed](hardware/wifi.h)
  - [The person detection sensor threshold (numbers beyond this number will be ignored)](integrations/presence.h)
  - [The person detection sensor tolerance (the distance between left and right for considering that you're at the center)](integrations/presence.h)
  - [The seconds it takes to display "No Data" when no MQTT packet is received](timeout.h)

### Run it
You can build it and flash it to the ESP-32 with platformio:

```
$ sudo pio run -t upload
```
