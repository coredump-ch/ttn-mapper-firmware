/**
 * Copyright (c) 2016 Danilo Bargen, Coredump Rapperswil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#define RX_PIN 7
#define TX_PIN 8
#define SERIAL_TIMEOUT 1000
#define USB_BAUD 57600
#define LORA_BAUD 57600
#define SECONDS_DELAY 15

#include <SoftwareSerial.h>


// Needed for SoftwareSerial
// https://github.com/queezythegreat/arduino-cmake/issues/91
extern "C" { void *__dso_handle = NULL; void *__cxa_atexit = NULL; }

// Serial port definition
SoftwareSerial loraSerial(RX_PIN, TX_PIN);

// Counter variable
uint32_t counter = 0;


/** Helper functions **/

void joinOtaa() {
    loraSerial.println("mac join otaa");
}

void sendPacket() {
    Serial.print("Sending packet ");
    Serial.println(counter, DEC);
//    loraSerial.print("mac tx uncnf 1 ");
//    loraSerial.println(counter++, DEC);
}

String readSerialLine() {
    String data = "";
    while (true) {
        while (!loraSerial.available()) {
            // Wait for data
        }
        char inByte = loraSerial.read();
        data += inByte;
        if (inByte == '\n') {
            return data;
        }
    }
}


/** Entry points **/

void setup() {

    // Set up USB serial
    Serial.setTimeout(SERIAL_TIMEOUT);
    Serial.begin(USB_BAUD);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Hello TTN Mapper!");

    // Set up RN2483 serial
    loraSerial.setTimeout(SERIAL_TIMEOUT);
    loraSerial.begin(LORA_BAUD);

    // Request debug information
    loraSerial.println("sys get ver");
    Serial.print("Version: ");
    Serial.println(readSerialLine());
    loraSerial.println("sys get vdd");
    Serial.print("Vdd: ");
    Serial.println(readSerialLine());
    loraSerial.println("mac get devaddr");
    Serial.print("Devaddr: ");
    Serial.println(readSerialLine());
    loraSerial.println("mac get deveui");
    Serial.print("Deveui: ");
    Serial.println(readSerialLine());
    loraSerial.println("mac get appeui");
    Serial.print("Appeui: ");
    Serial.println(readSerialLine());
    loraSerial.println("mac get status");
    Serial.print("Status: ");
    Serial.println(readSerialLine());
    loraSerial.println("radio get sf");
    Serial.print("Sf: ");
    Serial.println(readSerialLine());

    // Join network
//    joinOtaa();
}

void loop() {
    sendPacket();
    delay(SECONDS_DELAY * 1000);
}
