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

#define TTN_APP_EUI "70B3D57ED0000D7D"
#define TTN_APP_KEY "67E001FA4D013BEA53E41AB5AC83269C"

#define LED_PIN 13
#define SERIAL_TIMEOUT 20000
#define INFO_BAUD 57600
#define LORA_BAUD 57600
#define LORA_SF "sf12"
#define LORA_PWR 15
#define LORA_DATA_RATE 0
#define SECONDS_DELAY 15

#define INFO_SERIAL Serial
#define LORA_SERIAL Serial1
#define CRLF "\r\n"


// Counter variable
uint32_t counter = 1;

// Global serial buffer
String serialBuffer = "";


/** Helper functions **/

/**
 * Send a counter packet.
 */
void sendCounterPacket() {
    INFO_SERIAL.print("Sending packet ");
    INFO_SERIAL.print(counter, DEC);
    INFO_SERIAL.print("... ");
    LORA_SERIAL.print("mac tx uncnf 1 ");
    LORA_SERIAL.print(counter++, DEC);
    LORA_SERIAL.print(CRLF);
    readSerialLine();
    if (!serialBuffer.startsWith("ok")) {
        INFO_SERIAL.println(serialBuffer);
        fail("Could not send packet.");
    }
    readSerialLine();
    if (serialBuffer.startsWith("mac_tx_ok")) {
        INFO_SERIAL.println("ok");
    } else {
        INFO_SERIAL.println(serialBuffer);
        fail("Sending failed.");
    }
}

/**
 * Read a line from serial into the global buffer.
 * 
 * Does not include newline character.
 */
void readSerialLine() {
    serialBuffer = "";
    serialBuffer = LORA_SERIAL.readStringUntil('\n');
}

/**
 * Read a line from serial and print it immediately.
 */
void readAndPrint() {
    readSerialLine();
    INFO_SERIAL.println(serialBuffer);
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
    INFO_SERIAL.print("Error: ");
    INFO_SERIAL.println(msg);
    blinkFast(50);
    while (true); // TODO: Sleep
}


/** Entry points **/

void setup() {

    // Set up LED pin
    pinMode(LED_PIN, OUTPUT);
    blinkSlow(2);

    // Set up USB serial
    INFO_SERIAL.setTimeout(SERIAL_TIMEOUT);
    INFO_SERIAL.begin(INFO_BAUD);
    delay(2000);
    INFO_SERIAL.println("Hello TTN Mapper!");

    // Set up RN2483 serial
    LORA_SERIAL.setTimeout(SERIAL_TIMEOUT);
    LORA_SERIAL.begin(LORA_BAUD);
    blinkSlow(3);

    // Reset module
    INFO_SERIAL.println("Resetting module...  ");
    LORA_SERIAL.print("sys factoryRESET\r\n");
    readAndPrint();

    // Set network information
    LORA_SERIAL.print("mac set appeui ");
    LORA_SERIAL.print(TTN_APP_EUI);
    LORA_SERIAL.print(CRLF);
    readSerialLine();
    LORA_SERIAL.print("mac set appkey ");
    LORA_SERIAL.print(TTN_APP_KEY);
    LORA_SERIAL.print(CRLF);
    readSerialLine();
    LORA_SERIAL.print("mac save\r\n");
    readSerialLine();

    // Set radio params
/*    INFO_SERIAL.print("Setting SF to ");
    INFO_SERIAL.print(LORA_SF);
    INFO_SERIAL.print("... ");
    LORA_SERIAL.print("radio set sf ");
    LORA_SERIAL.print(LORA_SF);
    LORA_SERIAL.print(CRLF);
    readAndPrint();
    INFO_SERIAL.print("Setting power to ");
    INFO_SERIAL.print(LORA_PWR);
    INFO_SERIAL.print("... ");
    LORA_SERIAL.print("radio set pwr ");
    LORA_SERIAL.print(LORA_PWR);
    LORA_SERIAL.print(CRLF);
    readAndPrint();
    INFO_SERIAL.print("Setting data rate to ");
    INFO_SERIAL.print(LORA_DATA_RATE);
    INFO_SERIAL.print("... ");
    LORA_SERIAL.print("mac set dr ");
    LORA_SERIAL.print(LORA_DATA_RATE);
    LORA_SERIAL.print(CRLF);
    readAndPrint();*/
    
    // Request debug information

    Serial.println("-------------------------------");
    LORA_SERIAL.print("sys get vdd\r\n");
    INFO_SERIAL.print("Vdd: ");
    readAndPrint();
    LORA_SERIAL.print("mac get devaddr\r\n");
    INFO_SERIAL.print("Devaddr: ");
    readAndPrint();
    LORA_SERIAL.print("mac get deveui\r\n");
    INFO_SERIAL.print("Deveui: ");
    readAndPrint();
    LORA_SERIAL.print("mac get appeui\r\n");
    INFO_SERIAL.print("Appeui: ");
    readAndPrint();
    LORA_SERIAL.print("mac get status\r\n");
    INFO_SERIAL.print("Status: ");
    readAndPrint();
    LORA_SERIAL.print("mac get dr\r\n");
    INFO_SERIAL.print("Data rate: ");
    readAndPrint();
    LORA_SERIAL.print("radio get sf\r\n");
    INFO_SERIAL.print("Sf: ");
    readAndPrint();
    LORA_SERIAL.print("radio get pwr\r\n");
    INFO_SERIAL.print("Output power level: ");
    readAndPrint();
    INFO_SERIAL.println("-------------------------------");

    // Join network
    INFO_SERIAL.println("Joining via OTAA...");
    LORA_SERIAL.print("mac join otaa\r\n");
    readSerialLine();
    if (serialBuffer.startsWith("ok")) {
        INFO_SERIAL.print("Joining in process... ");
    } else {
        INFO_SERIAL.println(serialBuffer);
        fail("Could not join.");
    }
    readSerialLine();
    INFO_SERIAL.println(serialBuffer);
    if (serialBuffer.startsWith("accepted")) {
        INFO_SERIAL.println("Joined TTN via OTAA");
    } else {
        fail("Joining failed.");
    }

    blinkSlow(5);
}

void loop() {
    sendCounterPacket();
    blinkSlow(3);
    delay(SECONDS_DELAY * 1000);
    LORA_SERIAL.print("sys get vdd\r\n");
    INFO_SERIAL.print("Vdd: ");
    readAndPrint();
}
