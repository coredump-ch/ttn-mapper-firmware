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

#define TTN_APP_EUI ""
#define TTN_APP_KEY ""

#define RX_PIN 4 /*D4*/
#define TX_PIN 5 /*D5*/
#define LED_PIN 13
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
uint32_t counter = 1;

// Global serial buffer
String serialBuffer = "";


/** Helper functions **/

/**
 * Send a counter packet.
 */
void sendCounterPacket() {
    Serial.print("Sending packet ");
    Serial.print(counter, DEC);
    Serial.print("... ");
    loraSerial.print("mac tx uncnf 1 ");
    loraSerial.println(counter++, DEC);
    readSerialLine();
    if (!serialBuffer.startsWith("ok")) {
        Serial.println(serialBuffer);
        fail("Could not send packet.");
    }
    readSerialLine();
    if (serialBuffer.startsWith("mac_tx_ok")) {
        Serial.println("ok");
    } else {
        Serial.println(serialBuffer);
        fail("Sending failed.");
    }
}

/**
 * Read a line from serial into the global buffer.
 */
void readSerialLine() {
    serialBuffer = "";
    while (!loraSerial.available()) {
        delay(50);
    }
    while (loraSerial.available()) {
        char received = loraSerial.read();
        serialBuffer += received;
        if (received == '\n') {
            return;
        }
    }
}

/**
 * Read a line from serial and print it immediately.
 */
void readAndPrint() {
    readSerialLine();
    Serial.print(serialBuffer);
}

/**
 * Blink LED slowly (100ms delay)
 */
void blinkSlow(uint8_t count) {
    for (int i = 0; i < count; i++) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        delay(100);
    }
}

/**
 * Blink LED slowly (20ms delay)
 */
void blinkFast(uint8_t count) {
    for (int i = 0; i < count; i++) {
        digitalWrite(13, HIGH);
        delay(20);
        digitalWrite(13, LOW);
        delay(20);
    }
}

/**
 * Fail and stop.
 */
void fail(char *msg) {
    Serial.print("Error: ");
    Serial.println(msg);
    blinkFast(50);
    while (true); // TODO: Sleep
}


/** Entry points **/

void setup() {

    // Set up LED pin
    pinMode(LED_PIN, OUTPUT);
    blinkSlow(2);

    // Set up USB serial
    Serial.setTimeout(SERIAL_TIMEOUT);
    Serial.begin(USB_BAUD);
    delay(2000);
    Serial.println("Hello TTN Mapper!");

    // Set up RN2483 serial
    loraSerial.setTimeout(SERIAL_TIMEOUT);
    loraSerial.begin(LORA_BAUD);
    blinkSlow(3);

    // Reset module
    Serial.println("Resetting module...  ");
    loraSerial.print("sys factoryRESET\r\n");
    readAndPrint();

    // Set network information
    loraSerial.print("mac set appeui ");
    loraSerial.print(TTN_APP_EUI);
    loraSerial.println();
    readSerialLine();
    loraSerial.print("mac set appkey ");
    loraSerial.print(TTN_APP_KEY);
    loraSerial.println();
    readSerialLine();
    loraSerial.println("mac save");
    readSerialLine();
    
    // Request debug information
    
    Serial.println("-------------------------------");
    loraSerial.println("sys get vdd");
    Serial.print("Vdd: ");
    readAndPrint();
    loraSerial.println("mac get devaddr");
    Serial.print("Devaddr: ");
    readAndPrint();
    loraSerial.println("mac get deveui");
    Serial.print("Deveui: ");
    readAndPrint();
    loraSerial.println("mac get appeui");
    Serial.print("Appeui: ");
    readAndPrint();
    loraSerial.println("mac get status");
    Serial.print("Status: ");
    readAndPrint();
    loraSerial.println("radio get sf");
    Serial.print("Sf: ");
    readAndPrint();
    Serial.println("-------------------------------");

    // Join network
    loraSerial.println("mac join otaa");
    readSerialLine();
    if (!serialBuffer.startsWith("ok")) {
        Serial.println(serialBuffer);
        fail("Could not join.");
    }
    readSerialLine();
    if (serialBuffer.startsWith("accepted")) {
        Serial.println("Joined TTN via ABP");
    } else {
        Serial.println(serialBuffer);
        fail("Joining failed.");
    }

    blinkSlow(5);
}

void loop() {
    sendCounterPacket();
    blinkSlow(3);
    delay(SECONDS_DELAY * 1000);
}
