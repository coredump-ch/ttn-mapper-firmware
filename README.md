# TTN Mapper Firmware

Arduino firmware for use together with a RN2483 module.

http://ttnmapper.org/

When connected to a battery, the module will send a packet every 15 seconds.

## Initializing RN2483

First, the RN2483 needs to be initialized from a computer.

    sys factoryRESET
    mac set appeui <app-eui>
    mac get deveui
    mac set appkey <app-key>
    mac save
    mac join otaa

## Flashing Arduino Nano

Flash `main/main.ino` onto your Arduino board using the Arduino software.
