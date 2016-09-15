/**
 * Copyright (c) 2016 Danilo Bargen, Coredump Rapperswil
 *
 * Partially based on
 * https://www.hackster.io/makers-ns/lora-project-with-rn2483-and-particle-photon-435606
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
#define LOOP_DELAY_SECONDS 15
#define LORA_DATA_RATE 0
#define LORA_POWER 14
#define LED_PIN D7

int counter = 1;

SYSTEM_MODE(MANUAL);

void setup() {
    Serial1.begin(57600, SERIAL_8N1);
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    delay(2000);
    blink(3, 150);
    RN2483_init();
}

void loop() {
    Serial.printlnf("Sending packet %d:", counter++);
    String submitted = sendcmd("mac tx uncnf 1 42");
    if (!submitted.startsWith("ok")) {
        blink(1, 50);
        delay(1000);
        return;
    }
    while (!Serial1.available()) {
        delay(20);
    }
    String sent = Serial1.readStringUntil('\n');
    Serial.print("< ");
    Serial.println(sent);
    if (!sent.startsWith("mac_tx_ok")) {
        blink(1, 50);
        delay(1000);
        return;
    }
    blink(7, 30);
    delay(1000 * LOOP_DELAY_SECONDS);
}

String sendcmd_silent(String data) {
    Serial1.println(data); 
    while (!Serial1.available()) {
        delay(20);
    }
    return Serial1.readStringUntil('\n');
}

String sendcmd(String data) {
    Serial.println("> " + data);
    Serial1.println(data); 
    while (!Serial1.available()) {
        delay(20);
    }
    String line = Serial1.readStringUntil('\n');
    Serial.println("< " + line);
    return line;
}

void fail(String msg) {
    Serial.print("Failed: ");
    Serial.println(msg);
    blink(30, 30);
}

void RN2483_init() {
    delay(2000);
    Serial.println("Hello TTN Mapper!");
    sendcmd_silent(""); // Somehow needed to clear some buffer?

    sendcmd("sys factoryRESET");

    Serial.println("\r\nConfigure keys...");
    sendcmd(String::format("mac set appeui %s", TTN_APP_EUI));
    sendcmd(String::format("mac set appkey %s", TTN_APP_KEY));
    sendcmd("mac save");

    Serial.println("\r\nConfigure signal strength...");
    sendcmd(String::format("mac set dr %d", LORA_DATA_RATE));
    sendcmd(String::format("radio set pwr %d", LORA_POWER));

    Serial.println("\r\nGet some debug info...");
    sendcmd("mac get devaddr");
    sendcmd("mac get deveui");
    sendcmd("mac get appeui");
    sendcmd("mac get status");
    sendcmd("sys get vdd");
    sendcmd("mac get dr");
    sendcmd("radio get pwr");

    // Join the network via OTAA
    // Try until succeeded
    bool success = false;
    while (!success) {
        Serial.println("\r\nJoin the network...");
        Serial.println("> mac join otaa");
        String connecting = sendcmd_silent("mac join otaa");
        Serial.print("< ");
        if (!connecting.startsWith("ok")) {
            blink(1, 50);
            delay(1000);
            continue;
        }
        Serial.println(connecting);
        while (!Serial1.available()) {
            delay(20);
        }
        String joined = Serial1.readStringUntil('\n');
        if (!joined.startsWith("accepted")) {
            blink(1, 50);
            delay(1000);
            continue;
        }
        Serial.print("< ");
        Serial.println(joined);
        success = true;
        blink(2, 800);
    }

    Serial.println();
}

void blink(uint8_t count, uint32_t msDelay) {
    for (uint8_t i = 0; i < count; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(msDelay);
        digitalWrite(LED_PIN, LOW);
        delay(msDelay);
    }
}
