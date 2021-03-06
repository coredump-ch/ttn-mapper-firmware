# TTN Mapper Firmware

Arduino firmware for use together with a RN2483 module.

http://ttnmapper.org/

When connected to a battery, the module will send a packet every 15 seconds.

![Photo of a node](https://raw.github.com/coredump-ch/ttn-mapper-firmware/master/arduino/node.jpg)

## Flashing Arduino Nano

Edit `ttn-mapper-firmware.ino` and set the following variables:

- `TTN_APP_EUI`
- `TTN_APP_KEY`

You get these values by creating a new OTA activated device in the TTN
dashboard.

Then flash `ttn-mapper-firmware.ino` onto your Arduino board using the Arduino
software.

## License

MIT, see `LICENSE.txt`.
