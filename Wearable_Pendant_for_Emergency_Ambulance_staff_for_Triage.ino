#define BLYNK_TEMPLATE_ID "TMPL3MqsFqVbZ"
#define BLYNK_TEMPLATE_NAME "Wearable Pendant for Triage for Ambulance Staff"
#define BLYNK_AUTH_TOKEN "3pHWaC19i4kR6KLHLwQ4mBctCxz-RJDo"
#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
char ssid[] = "Divya";
char pass[] = "33333333";
#define VPIN_TRIAGE_STATE  V0
#define VPIN_TRIAGE_DESC   V1
#define VPIN_COMMAND       V2 
#define BUTTON_PIN 21
const int LED_RED    = 12;
const int LED_YELLOW = 13;
const int LED_GREEN  = 14;
const int LED_BLUE   = 15;
enum TriageState {
  MINOR = 0,     // Green
  DELAYED = 1,   // Yellow
  IMMEDIATE = 2, // Red
  DECEASED = 3   // Blue
};
const int ledPins[] = { LED_GREEN, LED_YELLOW, LED_RED, LED_BLUE };
const char* triageNames[] = { "Green", "Yellow", "Red", "Blue" };
const char* triageDesc[]  = { "Minor", "Delayed", "Immediate", "Deceased" };
TriageState current = MINOR;
const unsigned long DEBOUNCE_MS = 40;
const unsigned long LONG_MS     = 2000;
uint8_t stableState;
uint8_t lastRawState;
unsigned long lastEdgeTime = 0;
unsigned long pressStart   = 0;
bool longActionDone = false;
void updateLEDs() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(ledPins[i], (i == current) ? HIGH : LOW);
  }
}
void updateBlynk() {
  Blynk.virtualWrite(VPIN_TRIAGE_STATE, triageNames[current]);
  Blynk.virtualWrite(VPIN_TRIAGE_DESC, triageDesc[current]);
}
void cycleTriage() {
  current = (TriageState)((current + 1) % 4);
  updateLEDs();
  Serial.print("🔄 Triage state: ");
  Serial.println(triageNames[current]);
}
void confirmTriage() {
  Serial.print("✅ Triage state CONFIRMED: ");
  Serial.print(triageNames[current]);
  Serial.print(" - ");
  Serial.println(triageDesc[current]);
  updateBlynk();
}
BLYNK_WRITE(VPIN_COMMAND) {
  String cmd = param.asStr();
  cmd.toLowerCase();
  Serial.print("🎤 Speech command received: ");
  Serial.println(cmd);
  if (cmd.indexOf("green") != -1) {
    current = MINOR;
    updateLEDs();
    confirmTriage();
  } else if (cmd.indexOf("yellow") != -1) {
    current = DELAYED;
    updateLEDs();
    confirmTriage();
  } else if (cmd.indexOf("red") != -1) {
    current = IMMEDIATE;
    updateLEDs();
    confirmTriage();
  } else if (cmd.indexOf("blue") != -1 || cmd.indexOf("black") != -1) {
    current = DECEASED;
    updateLEDs();
    confirmTriage();
  } else if (cmd.indexOf("confirm") != -1) {
    confirmTriage();
  } else if (cmd.indexOf("next") != -1 || cmd.indexOf("cycle") != -1) {
    cycleTriage();
  } else {
    Serial.println("⚠️ Unknown speech command!");
  }
}
void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  updateLEDs();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  uint8_t r = digitalRead(BUTTON_PIN);
  stableState = r;
  lastRawState = r;
  lastEdgeTime = millis();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("🚑 Short press = cycle | Long press (>=2s) = confirm | Speech also works");
}
void loop() {
  Blynk.run();
  unsigned long now = millis();
  uint8_t raw = digitalRead(BUTTON_PIN);
  if (raw != lastRawState) {
    lastRawState = raw;
    lastEdgeTime = now;
  }
  if ((now - lastEdgeTime) > DEBOUNCE_MS && stableState != raw) {
    stableState = raw;
    if (stableState == LOW) {
      pressStart = now;
      longActionDone = false;
    } else {
      unsigned long dur = now - pressStart;
      if (!longActionDone && dur < LONG_MS) {
        cycleTriage();
      }
    }
  }
  if (stableState == LOW && !longActionDone) {
    unsigned long held = now - pressStart;
    if (held >= LONG_MS) {
      longActionDone = true;
      confirmTriage();
    }
  }
}
