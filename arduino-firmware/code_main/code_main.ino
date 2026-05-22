#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>

// --- SENSOR DEFINITIONS ---
// Axis 1 (Top & Bottom)
UltraSonicDistanceSensor L1_Sens1(23, 22);  // Lane 1, Front
UltraSonicDistanceSensor L1_Sens2(24, 25);  // Lane 1, Back
UltraSonicDistanceSensor L4_Sens1(28, 29);  // Lane 4, Front
UltraSonicDistanceSensor L4_Sens2(30, 31);  // Lane 4, Back

// Axis 2 (Left & Right)
UltraSonicDistanceSensor L2_Sens1(36, 37);  // Lane 2, Front
UltraSonicDistanceSensor L2_Sens2(34, 35);  // Lane 2, Back
UltraSonicDistanceSensor L3_Sens1(32, 33);  // Lane 3, Front
UltraSonicDistanceSensor L3_Sens2(26, 27);  // Lane 3, Back

// --- LED PIN DEFINITIONS ---
const int consoleLED = 13;  // Flashes during emergency
const int buttonPin = 12;   // Emergency Push Button (INPUT_PULLUP)

// Lane 1 (Top)
const int L1_G = 11, L1_Y = 10, L1_R = 9;
// Lane 4 (Bottom)
const int L4_G = 15, L4_Y = 14, L4_R = 16;
// Lane 2 (Left)
const int L2_G = 7, L2_Y = 8, L2_R = 6;
// Lane 3 (Right)
const int L3_G = 5, L3_Y = 4, L3_R = 3;

// --- LCD INITIALIZATION ---
LiquidCrystal_I2C lcd(0x27, 20, 4);

// --- TIMING CONSTANTS (in milliseconds) ---
const unsigned long TIME_EMPTY_OSCILLATE = 30000;  // 30s resting Green if no cars
const unsigned long TIME_NORMAL = 15000;           // 15s Green for normal traffic
const unsigned long TIME_CROWDED = 30000;          // 30s Green for heavy traffic
const unsigned long TIME_YELLOW = 5000;            // 5s Yellow transition

// --- DENSITY ENUM ---
enum Density { EMPTY = 0,
               NORMAL = 1,
               CROWDED = 2 };
Density axis1Status = EMPTY;
Density axis2Status = EMPTY;

// --- STATE MACHINE ENUMS ---
enum TrafficState {
  AXIS1_EVAL,
  AXIS1_GREEN,
  AXIS1_YELLOW,
  AXIS2_EVAL,
  AXIS2_GREEN,
  AXIS2_YELLOW
};
TrafficState currentState = AXIS1_EVAL;

// --- GLOBAL VARIABLES ---
unsigned long stateStartTime = 0;
unsigned long currentDuration = 0;
String activeAxisStr = "1 & 4";

// Background Scanning & UI Updates
unsigned long lastSensorRead = 0;
unsigned long lastLcdUpdate = 0;

// Emergency Debouncing
bool emergencyMode = false;
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long lastFlashTime = 0;
bool flashState = false;

void setup() {
  Serial.begin(115200);

  // Initialize LED Pins
  pinMode(consoleLED, OUTPUT);
  pinMode(L1_G, OUTPUT);
  pinMode(L1_Y, OUTPUT);
  pinMode(L1_R, OUTPUT);
  pinMode(L2_G, OUTPUT);
  pinMode(L2_Y, OUTPUT);
  pinMode(L2_R, OUTPUT);
  pinMode(L3_G, OUTPUT);
  pinMode(L3_Y, OUTPUT);
  pinMode(L3_R, OUTPUT);
  pinMode(L4_G, OUTPUT);
  pinMode(L4_Y, OUTPUT);
  pinMode(L4_R, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();

  // --- SPLASH SCREEN ---
  lcd.setCursor(0, 0);
  lcd.print("   Jemimah Asma'u   ");
  lcd.setCursor(0, 1);
  lcd.print("  Kyauta Thompson   ");
  lcd.setCursor(0, 2);
  lcd.print(" Matric: 21CK029344 ");
  // lcd.setCursor(0, 3);
  // lcd.print("  By: Deewansonic   ");
  delay(5000);
  lcd.clear();

  setAllRed();  // Initial safety state
}

void loop() {
  checkEmergencyButton();

  if (emergencyMode) {
    handleEmergencyMode();
  } else {
    digitalWrite(consoleLED, HIGH);  // Solid Green = Normal Auto Mode

    // Read sensors continuously in the background without blocking
    if (millis() - lastSensorRead >= 300) {
      lastSensorRead = millis();
      scanAllLanes();
    }

    manageTrafficLogic();
    updateLCD();
  }
}

// --- CORE SENSOR LOGIC ---
Density getLaneDensity(float frontCm, float backCm) {
  if (frontCm < 0) frontCm = 999;
  if (backCm < 0) backCm = 999;

  if (frontCm < 12.0 && backCm < 12.0) return CROWDED;
  if (frontCm < 12.0 || backCm < 12.0) return NORMAL;
  return EMPTY;
}

void scanAllLanes() {
  Density L1 = getLaneDensity(L1_Sens1.measureDistanceCm(), L1_Sens2.measureDistanceCm());
  Density L4 = getLaneDensity(L4_Sens1.measureDistanceCm(), L4_Sens2.measureDistanceCm());
  axis1Status = max(L1, L4);  // If one is normal and one is crowded, Axis is Crowded

  Density L2 = getLaneDensity(L2_Sens1.measureDistanceCm(), L2_Sens2.measureDistanceCm());
  Density L3 = getLaneDensity(L3_Sens1.measureDistanceCm(), L3_Sens2.measureDistanceCm());
  axis2Status = max(L2, L3);
}

// --- STATE MACHINE LOGIC ---
void manageTrafficLogic() {
  unsigned long elapsed = millis() - stateStartTime;

  switch (currentState) {

    // ======== AXIS 1 (LANE 1 & 4) ========
    case AXIS1_EVAL:
      activeAxisStr = "1 & 4 (MAIN)";
      currentDuration = (axis1Status == CROWDED) ? TIME_CROWDED : (axis1Status == NORMAL) ? TIME_NORMAL
                                                                                          : TIME_EMPTY_OSCILLATE;
      triggerAxis1Green();
      currentState = AXIS1_GREEN;
      break;

    case AXIS1_GREEN:
      // SMART INTERRUPT: If Axis 1 is Empty but a car arrives on Axis 2, skip the wait!
      if (axis1Status == EMPTY && axis2Status > EMPTY) {
        currentDuration = 0;
      }
      if (elapsed >= currentDuration) {
        triggerAxis1Yellow();
        currentState = AXIS1_YELLOW;
      }
      break;

    case AXIS1_YELLOW:
      if (elapsed >= TIME_YELLOW) {
        currentState = AXIS2_EVAL;
      }
      break;

    // ======== AXIS 2 (LANE 2 & 3) ========
    case AXIS2_EVAL:
      activeAxisStr = "2 & 3 (SIDE)";
      currentDuration = (axis2Status == CROWDED) ? TIME_CROWDED : (axis2Status == NORMAL) ? TIME_NORMAL
                                                                                          : TIME_EMPTY_OSCILLATE;
      triggerAxis2Green();
      currentState = AXIS2_GREEN;
      break;

    case AXIS2_GREEN:
      // SMART INTERRUPT: If Axis 2 is Empty but a car arrives on Axis 1, skip the wait!
      if (axis2Status == EMPTY && axis1Status > EMPTY) {
        currentDuration = 0;
      }
      if (elapsed >= currentDuration) {
        triggerAxis2Yellow();
        currentState = AXIS2_YELLOW;
      }
      break;

    case AXIS2_YELLOW:
      if (elapsed >= TIME_YELLOW) {
        currentState = AXIS1_EVAL;
      }
      break;
  }
}

// --- HARDWARE LIGHT SWITCHES ---
void setAllRed() {
  digitalWrite(L1_R, HIGH);
  digitalWrite(L1_Y, LOW);
  digitalWrite(L1_G, LOW);
  digitalWrite(L4_R, HIGH);
  digitalWrite(L4_Y, LOW);
  digitalWrite(L4_G, LOW);
  digitalWrite(L2_R, HIGH);
  digitalWrite(L2_Y, LOW);
  digitalWrite(L2_G, LOW);
  digitalWrite(L3_R, HIGH);
  digitalWrite(L3_Y, LOW);
  digitalWrite(L3_G, LOW);
}

void triggerAxis1Green() {
  setAllRed();  // Make sure everything is Red first
  // Turn Axis 1 Green
  digitalWrite(L1_R, LOW);
  digitalWrite(L1_G, HIGH);
  digitalWrite(L4_R, LOW);
  digitalWrite(L4_G, HIGH);
  stateStartTime = millis();
}

void triggerAxis1Yellow() {
  // Axis 1 Green turns OFF, Yellow turns ON. Axis 2 stays Red.
  digitalWrite(L1_G, LOW);
  digitalWrite(L1_Y, HIGH);
  digitalWrite(L4_G, LOW);
  digitalWrite(L4_Y, HIGH);
  stateStartTime = millis();
}

void triggerAxis2Green() {
  setAllRed();  // Make sure Axis 1 goes back to Red
  // Turn Axis 2 Green
  digitalWrite(L2_R, LOW);
  digitalWrite(L2_G, HIGH);
  digitalWrite(L3_R, LOW);
  digitalWrite(L3_G, HIGH);
  stateStartTime = millis();
}

void triggerAxis2Yellow() {
  // Axis 2 Green turns OFF, Yellow turns ON. Axis 1 stays Red.
  digitalWrite(L2_G, LOW);
  digitalWrite(L2_Y, HIGH);
  digitalWrite(L3_G, LOW);
  digitalWrite(L3_Y, HIGH);
  stateStartTime = millis();
}

// --- LCD UPDATES ---
void updateLCD() {
  if (millis() - lastLcdUpdate >= 250) {
    lastLcdUpdate = millis();

    int timeLeft = 0;
    String modeType = "YELLOW";
    Density currentDisplayStatus = EMPTY;

    if (currentState == AXIS1_GREEN || currentState == AXIS1_EVAL) {
      timeLeft = (currentDuration - (millis() - stateStartTime)) / 1000;
      modeType = "GREEN ";
      currentDisplayStatus = axis1Status;
    } else if (currentState == AXIS2_GREEN || currentState == AXIS2_EVAL) {
      timeLeft = (currentDuration - (millis() - stateStartTime)) / 1000;
      modeType = "GREEN ";
      currentDisplayStatus = axis2Status;
    } else {
      timeLeft = (TIME_YELLOW - (millis() - stateStartTime)) / 1000;
    }

    if (timeLeft < 0) timeLeft = 0;  // Prevent negatives

    String densStr = (currentDisplayStatus == CROWDED) ? "CROWDED" : (currentDisplayStatus == NORMAL) ? "NORMAL "
                                                                                                      : "EMPTY  ";

    lcd.setCursor(0, 0);
    lcd.print("AXIS: ");
    lcd.print(activeAxisStr);
    lcd.print("    ");

    lcd.setCursor(0, 1);
    lcd.print("TRAFFIC: ");
    lcd.print(densStr);
    lcd.print("   ");

    lcd.setCursor(0, 2);
    lcd.print(modeType);
    lcd.print(" TIME: ");
    lcd.print(timeLeft);
    lcd.print("s   ");

    lcd.setCursor(0, 3);
    lcd.print("SYS: SMART ADAPTIVE ");
  }
}

// --- EMERGENCY OVERRIDE ---
void checkEmergencyButton() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) { lastDebounceTime = millis(); }
  if ((millis() - lastDebounceTime) > 50) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        emergencyMode = !emergencyMode;
        if (emergencyMode) {
          setAllRed();
          lcd.clear();
        } else {
          lcd.clear();
        }
      }
    }
  }
  lastButtonState = reading;
}

void handleEmergencyMode() {
  if (millis() - lastFlashTime >= 200) {
    lastFlashTime = millis();
    flashState = !flashState;
    digitalWrite(consoleLED, flashState ? HIGH : LOW);
  }
  if (millis() - lastLcdUpdate >= 1000) {
    lastLcdUpdate = millis();
    lcd.setCursor(0, 0);
    lcd.print("!!! EMERGENCY !!!   ");
    lcd.setCursor(0, 1);
    lcd.print("ALL LANES HALTED    ");
    lcd.setCursor(0, 2);
    lcd.print("WAITING TO RESUME...");
    lcd.setCursor(0, 3);
    lcd.print("PRESS BTN TO CANCEL ");
  }
}