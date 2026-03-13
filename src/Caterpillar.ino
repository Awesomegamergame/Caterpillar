#include <Arduino.h>
#include <LiquidCrystal_I2C.h>   // Library for I2C LCD

// I2C connections
// SDA -> A4
// SCL -> A5
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD address and size

// Ultrasonic sensor pins
const int trigLeft = 2;
const int echoLeft = 3;

const int trigRight = 4;
const int echoRight = 5;

// Motor driver pins (H-bridge)
#define IN1 6
#define IN2 7
#define IN3 9
#define IN4 10

int distanceLeft;
int distanceRight;

int followRange = 40;   // Maximum distance to follow (cm)
int stopRange = 10;     // Minimum safe distance (cm)


// Measure distance using ultrasonic sensor
int measureDistance(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  int distance = duration * 0.034 / 2;

  return distance;
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
}


void loop() {

  // Measure distances
  distanceLeft = measureDistance(trigLeft, echoLeft);
  distanceRight = measureDistance(trigRight, echoRight);

  // Serial output
  Serial.print("Left: ");
  Serial.print(distanceLeft);
  Serial.print(" cm | Right: ");
  Serial.print(distanceRight);
  Serial.println(" cm");


  // Display distances on LCD
  lcd.setCursor(0,0);
  lcd.print("L:");
  lcd.print(distanceLeft);
  lcd.print(" R:");
  lcd.print(distanceRight);
  lcd.print("   "); // clears extra digits

  // Move forward
  if (distanceLeft < followRange && distanceRight < followRange &&
      distanceLeft > stopRange && distanceRight > stopRange) {

      Serial.println("Move Forward");
      lcd.setCursor(0,1);
      lcd.print("Forward       ");

      moveForward();
  }

  // Turn left
  else if (distanceLeft < distanceRight && distanceLeft < followRange) {

      Serial.println("Turn Left");
      lcd.setCursor(0,1);
      lcd.print("Turn Left     ");

      turnLeft();
  }

  // Turn right
  else if (distanceRight < distanceLeft && distanceRight < followRange) {

      Serial.println("Turn Right");
      lcd.setCursor(0,1);
      lcd.print("Turn Right    ");

      turnRight();
  }

  // Stop if too close
  else if (distanceLeft <= stopRange || distanceRight <= stopRange) {

      Serial.println("Stop - Too Close");
      lcd.setCursor(0,1);
      lcd.print("Too Close     ");

      stopMotors();
  }

  // Idle if nothing detected
  else {

      Serial.println("Idle");
      lcd.setCursor(0,1);
      lcd.print("No Target     ");

      stopMotors();
  }

  delay(200);
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