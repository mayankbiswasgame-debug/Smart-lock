#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

//RFID pins
#define ss_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
Servo lockServo;

// Pins
const int greenLED = 5;
const int redLED = 6;

// Ultrasonic pins (kept but not used)
const int trigPin = 7;
const int echoPin = 8;

// Servo positions
const int lockPos = 0;
const int unlockPos = 90;

// Your RFID UID 
byte validUID[4] = {0xA3, 0x67, 0x7C, 0x05};

unsigned long lastScanTime = 0;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCB Init();

    lockServo.attach(3);
    lockServo.write(lockPos);

    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    
    // Pins still initialized (but not used)
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    Serial.println("System Ready - Scan Card");
}

// UID Check
bool isValidCard(byte *uid) {
    for (byte i = 0; i < 4; i++) {
        if (uid[i] != validUID[i]) return false;
    }
    return true;
}

void loop() {

    // Prevent rapid multiple scans
    if (millis() - lastScanTime < 2000) return;

    // RFID Detection
    if (!rfid.PICC IsNewCardPresent()) return;
    if (!rfid.PICC ReadCardSerial()) return;

    lastScanTime = millis();

    Serial.print("UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // Correct Card
    if (isValidCard(rfid.uid.uidByte)) {
        Serial.println("Access Granted");

        digitlWrite(greenLED, HIGH);

        lockServo.write(unlockPos);
        delay(3000);
        lockServo.write(lockPos);

        digitlWrite(greenLED, HIGH);
    }
    // Wrong Card
    else {
        Serial.println("Access Denide");

        digitlWrite(redLED, HIGH);
        delay(2000);
        digitlWrite(redLED, LOW);
    }

    rfid.PICC HaltA();
}