#include <LiquidCrystal.h>

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int gasSensor = A0;
int ledPin = 13;
int buzzer = 10;
int threshold = 250;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Gas Monitoring");
  delay(2000);
  lcd.clear();
}

void loop() {
  int gasValue = analogRead(gasSensor);

  lcd.setCursor(0, 0);
  lcd.print("Gas: ");
  lcd.print(gasValue);
  lcd.print("   ");

  if (gasValue >= threshold) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzer, HIGH);
    Serial.print(gasValue);
    Serial.println(" |Gas detected|");

    lcd.setCursor(0, 1);
    lcd.print(" Gas Detected! ");
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzer, LOW);
    Serial.print("sensorValue: ");
    Serial.println(gasValue);

    lcd.setCursor(0, 1);
    lcd.print("Safe Environment ");
  }

  delay(1000);
}
