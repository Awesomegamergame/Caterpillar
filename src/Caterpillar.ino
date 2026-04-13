#include <Arduino.h>
#include <LiquidCrystal_I2C.h>   // Library for I2C LCD
#include <SPI.h>
#include <MFRC522.h>
#include "Sound.h"
#include "LedMatrix.h"

// I2C connections
// SDA -> A4
// SCL -> A5
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD address and size

void printUid(const MFRC522::Uid &uid);
int measureDistance(int trigPin, int echoPin);
void moveForward();
void turnLeft();
void turnRight();
void stopMotors();
void updateDistanceDisplay(int distanceLeft, int distanceRight);
void applyMotionFromDistances(int distanceLeft, int distanceRight);
void readAndHandleRfid();

// Passive buzzer / speaker
// Wire: BUZZER_PIN -> (+) buzzer, other pin -> GND (optional 100-220Ω series resistor)
const uint8_t BUZZER_PIN = A1;

Sound buzzer(BUZZER_PIN);

// MAX7219 matrix pins.
// DIN and CLK are shared with SPI pins used by RC522.
const uint8_t MATRIX_DIN_PIN = 11;
const uint8_t MATRIX_CLK_PIN = 13;
const uint8_t MATRIX_CS_PIN = A2;
const uint8_t EYE_BRIGHTNESS = 0; // 0 (dimmest) to 15 (brightest)

EyeMatrix eye(MATRIX_DIN_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN);

// Ultrasonic sensor pins
const int trigLeft = 2;
const int echoLeft = 3;

const int trigRight = 4;
const int echoRight = 5;

// Motor driver pins (H-bridge)
constexpr uint8_t IN1 = 6;
constexpr uint8_t IN2 = 7;
constexpr uint8_t IN3 = 9;
constexpr uint8_t IN4 = 10;

const int followRange = 40;   // Maximum distance to follow (cm)
const int stopRange = 10;     // Minimum safe distance (cm)
const unsigned long CONTROL_INTERVAL_MS = 200;
unsigned long lastControlMs = 0;

// RC522 RFID
//Pins for RC522:
// SDA/SS -> D8
// RST    -> A0
// MOSI   -> D11
// MISO   -> D12
// SCK    -> D13
constexpr uint8_t RC522_SS_PIN  = 8;
constexpr uint8_t RC522_RST_PIN = A0;
MFRC522 rfid(RC522_SS_PIN, RC522_RST_PIN);

// Measure distance using ultrasonic sensor
int measureDistance(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  return static_cast<int>(duration * 0.034 / 2);
}

void setup() {

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);

  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  buzzer.begin();
  eye.begin();
  eye.setBrightness(EYE_BRIGHTNESS);

  // Initialize RFID
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RC522 ready. Tap a tag/card...");
}


void loop() {

  eye.update();

  const unsigned long now = millis();
  if ((now - lastControlMs) >= CONTROL_INTERVAL_MS) {
    lastControlMs = now;

    const int distanceLeft = measureDistance(trigLeft, echoLeft);
    const int distanceRight = measureDistance(trigRight, echoRight);

    updateDistanceDisplay(distanceLeft, distanceRight);
    applyMotionFromDistances(distanceLeft, distanceRight);
  }

  readAndHandleRfid();
}

void updateDistanceDisplay(int distanceLeft, int distanceRight) {
  Serial.print("Left: ");
  Serial.print(distanceLeft);
  Serial.print(" cm | Right: ");
  Serial.print(distanceRight);
  Serial.println(" cm");

  lcd.setCursor(0,0);
  lcd.print("L:");
  lcd.print(distanceLeft);
  lcd.print(" R:");
  lcd.print(distanceRight);
  lcd.print("   "); // clears extra digits
}

void applyMotionFromDistances(int distanceLeft, int distanceRight) {
  if (distanceLeft < followRange && distanceRight < followRange &&
      distanceLeft > stopRange && distanceRight > stopRange) {
    Serial.println("Move Forward");
    lcd.setCursor(0,1);
    lcd.print("Forward       ");
    moveForward();
    return;
  }

  if (distanceLeft < distanceRight && distanceLeft < followRange) {
    Serial.println("Turn Left");
    lcd.setCursor(0,1);
    lcd.print("Turn Left     ");
    turnLeft();
    return;
  }

  if (distanceRight < distanceLeft && distanceRight < followRange) {
    Serial.println("Turn Right");
    lcd.setCursor(0,1);
    lcd.print("Turn Right    ");
    turnRight();
    return;
  }

  if (distanceLeft <= stopRange || distanceRight <= stopRange) {
    Serial.println("Stop - Too Close");
    lcd.setCursor(0,1);
    lcd.print("Too Close     ");
    stopMotors();
    return;
  }

  Serial.println("Idle");
  lcd.setCursor(0,1);
  lcd.print("No Target     ");
  stopMotors();
}

void readAndHandleRfid() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID: ");
  printUid(rfid.uid);
  Serial.println();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// Motor movement functions
void moveForward() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Print UID function
void printUid(const MFRC522::Uid &uid) {
  for (byte i = 0; i < uid.size; i++) {
    if (uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(uid.uidByte[i], HEX);
    if (i + 1 < uid.size) Serial.print(":");
  }
}
