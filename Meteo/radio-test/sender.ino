#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <string.h>

RF24 radio(14,15);

const byte rxAddr[6] = "00001";

void setup() {
    radio.begin();
    radio.setRetries(15, 15);
    radio.openWritingPipe(rxAddr);

    radio.stopListening();
    Serial.begin(9600);
    Serial.println("Zaczynam nadawac...");
}

String int_to_str(int i) {
    String num = "";
    while (i>0) {
        num = (char)(i%10 + '0') + num;
        i /= 10;
    }
    return num;
}


void loop() {

    int i = 1;
    String text;
    while(true) {
        text = int_to_str(i);
        i += 2;
        radio.write(&text, text.length());
        Serial.println("Nadaję: " + text);
        delay(1000);
    }
}

