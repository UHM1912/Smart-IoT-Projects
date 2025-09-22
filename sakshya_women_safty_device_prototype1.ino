// === Sakshya IoT Safety Device — Final Revised Sketch ===
const int PIN_MOTION   = A0;   // Pot: motion
const int PIN_HR       = A1;   // Pot: heart-rate
const int PIN_SOUND    = A2;   // Pot: sound
const int PIN_BTN      = 2;    // Push button (INPUT_PULLUP)
const int PIN_SAFE     = 3;    // Slide switch (INPUT_PULLUP) - Safe Zone
const int PIN_BUZZ     = 9;    // Buzzer (use tone/noTone)
const int PIN_STROBE   = 10;   // LED (strobe)

const int MOTION_THRESH = 500; // adjust by testing
const int HR_THRESH     = 500;
const int SOUND_THRESH  = 500;

const unsigned long CONFIRM_MS = 5000UL;   // confirmation window (ms)
const unsigned long LONGPRESS_MS = 1500UL; // long press threshold (ms)
const unsigned long BTN_DEBOUNCE_MS = 50UL;

unsigned long btnLastChange = 0;
int btnLastState = HIGH;         // for INPUT_PULLUP: released = HIGH
unsigned long btnPressTime = 0;
bool btnWasLongPressHandled = false;

void setup() {
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_SAFE, INPUT_PULLUP);
  pinMode(PIN_BUZZ, OUTPUT);
  pinMode(PIN_STROBE, OUTPUT);
  digitalWrite(PIN_BUZZ, LOW);
  digitalWrite(PIN_STROBE, LOW);

  Serial.begin(9600);
  delay(200);
  Serial.println(F("=== Sakshya IoT Safety Device Started ==="));

  // quick self-test buzzer + LED
  tone(PIN_BUZZ, 1000, 300);
  digitalWrite(PIN_STROBE, HIGH);
  delay(350);
  digitalWrite(PIN_STROBE, LOW);
  delay(150);
  Serial.println(F("Self-test complete."));
}

// helper to read button with debounce and manage long press detection
// returns: 0 = nothing, 1 = short press event, 2 = long press event
int handleButton() {
  int raw = digitalRead(PIN_BTN);
  unsigned long now = millis();

  if (raw != btnLastState) {
    if (now - btnLastChange > BTN_DEBOUNCE_MS) {
      btnLastChange = now;
      btnLastState = raw;
      if (raw == LOW) {
        btnPressTime = now;
        btnWasLongPressHandled = false;
      } else {
        unsigned long hold = now - btnPressTime;
        if (hold >= LONGPRESS_MS) {
          if (!btnWasLongPressHandled) {
            btnWasLongPressHandled = true;
            return 2;
          }
          return 0;
        } else {
          return 1;
        }
      }
    }
  } else {
    if (raw == LOW && !btnWasLongPressHandled && (now - btnPressTime >= LONGPRESS_MS)) {
      btnWasLongPressHandled = true;
      return 2;
    }
  }
  return 0;
}

// Activate alarm: buzzer + strobe pattern for a certain duration
void activateAlarmSequence(unsigned long durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    digitalWrite(PIN_STROBE, HIGH);
    tone(PIN_BUZZ, 1200);
    delay(120);
    digitalWrite(PIN_STROBE, LOW);
    noTone(PIN_BUZZ);
    delay(80);

    if (digitalRead(PIN_SAFE) == HIGH) {
      Serial.println(F("Safe Zone turned ON during alarm -> stopping alarm."));
      noTone(PIN_BUZZ);
      digitalWrite(PIN_STROBE, LOW);
      return;
    }

    int btnEvent = handleButton();
    if (btnEvent == 2) {
      Serial.println(F("Long button press detected during alarm -> stopping alarm (user override)."));
      noTone(PIN_BUZZ);
      digitalWrite(PIN_STROBE, LOW);
      return;
    }
  }
  noTone(PIN_BUZZ);
  digitalWrite(PIN_STROBE, LOW);
}

void loop() {
  int motion = analogRead(PIN_MOTION);
  int hr = analogRead(PIN_HR);
  int sound = analogRead(PIN_SOUND);

  bool safeZone = (digitalRead(PIN_SAFE) == HIGH);
  Serial.print(F("Motion: ")); Serial.print(motion);
  Serial.print(F(" | HR: ")); Serial.print(hr);
  Serial.print(F(" | Sound: ")); Serial.print(sound);
  Serial.print(F(" | SafeZone: ")); Serial.println(safeZone ? F("YES") : F("NO"));

  int btnEvent = handleButton();
  if (btnEvent == 2) { 
    Serial.println(F("Long Press detected -> MANUAL SOS ACTIVATED"));
    activateAlarmSequence(6000);
    delay(300);
    return;
  }

  if (safeZone) {
    noTone(PIN_BUZZ);
    digitalWrite(PIN_STROBE, LOW);
    delay(250);
    return;
  }

  bool threat = (motion > MOTION_THRESH) && ((sound > SOUND_THRESH) || (hr > HR_THRESH));
  if (threat) {
    Serial.println(F("** Potential threat detected -> confirmation window started **"));
    unsigned long start = millis();
    bool cancelled = false;

    while (millis() - start < CONFIRM_MS) {
      digitalWrite(PIN_STROBE, ((millis() / 200) % 2) == 0 ? HIGH : LOW);

      int ev = handleButton();
      if (ev == 1) {
        cancelled = true;
        Serial.println(F("Short press detected -> ALERT CANCELLED by user"));
        break;
      }
      if (ev == 2) {
        Serial.println(F("Long press detected during confirm -> MANUAL SOS (immediate)"));
        cancelled = false;
        break;
      }

      if (digitalRead(PIN_SAFE) == HIGH) {
        cancelled = true;
        Serial.println(F("Safe Zone activated during confirmation -> Cancelled"));
        break;
      }
    }

    digitalWrite(PIN_STROBE, LOW);

    if (cancelled) {
      noTone(PIN_BUZZ);
      delay(200);
      return;
    }

    Serial.println(F("!!! EMERGENCY CONFIRMED -> Triggering alarm sequence !!!"));
    activateAlarmSequence(8000);
    Serial.println(F("Alarm sequence completed."));
  }

  delay(200);
}
