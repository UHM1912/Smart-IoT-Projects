#include <LiquidCrystal.h>

// LCD pin mapping: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// PIR moved to pin 6 (to free up pin 2 for LCD D7)
const int pirPin = 6;
const int ledPin = 13;
const int buzzerPin = 8;

// RGB LED pins
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("System Ready");

  Serial.begin(9600);
  delay(2000);
  lcd.clear();
}

void loop() {
  int motion = digitalRead(pirPin);

  if (motion == HIGH) {
    Serial.println("Motion Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    setRGBColor(255, 0, 0);  // RED

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected");
    lcd.setCursor(0, 1);
    lcd.print("Someone in house");
  } else {
    Serial.println("No motion.");
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    setRGBColor(0, 255, 0);  // GREEN

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("House is empty");
  }

  delay(500);
}

void setRGBColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
