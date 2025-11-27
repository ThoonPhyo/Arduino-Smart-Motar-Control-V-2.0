/*

======== AUTO WATER MOTAR CONTROL V-2 =======

Function Modes

1. Auto Mode (No save voltage)
  - Motar Run when water level low than 100%  
  - Motar Stop when water level is 100%

2. Manual Mode
  - Motar relay always open, you need to control by on/off swithches of Motar
  - Not related with sensor 
  - This Mode is prefer for external usages that not related water level sensor

3. Auto Save Mode (Save voltage)
  - Motar Run when water level low and voltage is higher than save voltage
  - Motar Stop when water level is 100% or current voltage is lower than save voltage
  - This Mode is perfer for Motar's saving by using low voltage protection

  - *Low Voltage Protection*
    - When current ACV low than SV, Motar will Stop.
    - If this situation happens more than 2 time quickly within 1 min, system will sure that is low voltage Problem.
    - If low votage problem found, system will wait perfect safe voltage to restart the motar. 
    - Motar will restart when current ACV is equal or higher than safe voltage.


Protections and Adjustments

- If no water flow after 1 min of motar start in auto save and no save modes, motar will stop immediately and lcd will show "No Water Flowing"

- You can adjust the save voltage(SV) in auto save mode by pressing the adjust btn for 3s.
  Adjust the SV by Potentionmeter and then press the adjust button for 3s to release.

  
*/





#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

// ================== LCD ==================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================== RGB LED Pins ==================
#define RED_PIN     11
#define GREEN_PIN   10
#define BLUE_PIN    9

// ================== Mode Switches ==================
#define MODE_SAVE   4
#define MODE_NOSAVE 5

// ================== Water Flow Detection sensor ==================
#define WATER_FLOW_DETECTION 12

// ================== Float Switches ==================
#define FLOAT_FULL  3
#define FLOAT_HALF  2

// ================== Buzzer ==================
#define BUZZER      13

// ================== Motor Relay & LED ==================
#define MOTOR_RELAY 6
#define MOTOR_LED   8

// ================== Adjustment Controls ==================
#define ADJUST_BTN  7
#define ADJUST_POT  A1

// ================== AC volt sensor ==================
#define SENSOR_PIN A0   // Analog pin connected to voltage sensor

// ****************** User Adjusable Veriables ******************
int motar_QS_check_delay = 8000; // Motar quick stop check delay
int low_V_check_delay = 60000; // low voltage problem check delay ; default is 1 min
int water_flow_check_delay = 7000; // water flow check delay after motar started

// ================== System State ==================
// 0=Save, 1=NoSave, 2=Manual 
int previous_mode = 0;
int currentMode = -1;
int prevMode    = -1;
bool showingMode = false; // lcd show mode changing
unsigned long modeDisplayTime = 0;

unsigned long prevVoltMillis = 0; // lcd refresh changed ac Volts
const long voltInterval = 500;  // lcd refresh every 500 mills
int voltage = 0;

// Water Flow Detection
bool water_flow_issue = false;  //water flow issue confirm
unsigned long Motar_run_time = 0; // check water flow after motar start

// Water Level
bool full  = false; // Float 100%
bool half  = false; // Float 50%

// Motor State
bool motorState = false; 

// Water full alarm
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

// Adjust Mode Variables
bool adjustMode = false;
unsigned long adjustBtnPressStart = 0;
int save_voltage = 0;   // from EEPROM
int new_voltage = 0;    // voltage after adjust
unsigned long verifiedStart = 0;    // after adjust verify
bool Verified_showing = false;  // lcd show verified after adjust

// AC Voltage sensor calibration
// Calibration values
int adc_max = 778;       // Maximum sensor value during calibration
int adc_min = 238;       // Minimum sensor value during calibration
float volt_multi = 183;  // RMS voltage from multimeter
float volt_multi_p;      // Peak voltage
float volt_multi_n;      // Negative peak voltage
float volt_rms;          // Measured RMS voltage

// Low AC Voltage Problem Variables
int Start_voltage = 0; // noted motar start voltage
int Stop_voltage = 0; // noted motar stop voltage
int firstRunStartVoltage = 0; // <-- NEW: preserve first start voltage
bool low_voltage_problem = false; // detect low problem
int problem_count = 0;  // to confrim low voltage problem sure
int dropped_voltage = 0; // drop voltage from save voltage
int safe_voltage = 0;  // perfect voltage to motar restart after low voltage problem
bool detect_low_volt_problem = false; // detection start or not
bool showSafeMsg = false;              // start showing "Safe V reached
int safeMsgStart = 0; // lcd show safe voltage got

// ==================================================
// SETUP
// ==================================================
void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();

  // RGB LED
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Mode Swithces
  pinMode(MODE_SAVE,   INPUT_PULLUP);
  pinMode(MODE_NOSAVE, INPUT_PULLUP);

  // Water flow Detection
  pinMode(WATER_FLOW_DETECTION, INPUT_PULLUP);

  // Float Switches for water level Sensing
  pinMode(FLOAT_FULL, INPUT_PULLUP);
  pinMode(FLOAT_HALF, INPUT_PULLUP);

  // Alarm Buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // Motar Relay & LED
  pinMode(MOTOR_RELAY, OUTPUT);
  pinMode(MOTOR_LED, OUTPUT);
  digitalWrite(MOTOR_RELAY, LOW);
  digitalWrite(MOTOR_LED, LOW);

  // Adjust BTN & Potentionmeter
  pinMode(ADJUST_BTN, INPUT_PULLUP);
  pinMode(ADJUST_POT, INPUT);

  // AC Voltage 
  volt_multi_p = volt_multi * 1.4142;
  volt_multi_n = -volt_multi_p;

  Serial.begin(9600);

  // Load saved voltage from EEPROM
  EEPROM.get(0, save_voltage);
  if (save_voltage < 0 || save_voltage > 230) save_voltage = 160;

  lcd.setCursor(0,0);
  lcd.print("System Starting...");
  delay(1000);
  lcd.clear();
}

// ==================================================
// MAIN LOOP
// ==================================================
void loop() {
  readModeSwitches();
  readWaterLevel();
  updateVoltage();
  updateModeDisplay();
  handleAdjust();
  updateLCD();
  updateRGB();
  waterFullAlarm();
  motorControl();
  AC_Sensor();
}

// ===================== FUNCTIONS =====================

// ---------- RGB LED ----------
void setColor(int r, int g, int b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

// ---------- Mode Handling ----------
void readModeSwitches() {
  int save   = digitalRead(MODE_SAVE);
  int nosave = digitalRead(MODE_NOSAVE);

  currentMode = -1;
  if (save == LOW) currentMode = 0;
  else if (nosave == LOW) currentMode = 1;
  else if (save == HIGH && nosave == HIGH) currentMode = 2;

  if (currentMode != prevMode && currentMode != -1) {
    prevMode = currentMode;
    modeDisplayTime = millis();
    showingMode = true;

    lcd.clear();
    if (currentMode == 0) { setColor(0, 255, 0); lcd.print("Save Auto Mode"); }
    if (currentMode == 1) { setColor(255, 0, 0); lcd.print("No Save Auto Mode"); }
    if (currentMode == 2) { setColor(0, 0, 255); lcd.print("Manual Mode"); }
  }
}


void updateModeDisplay() {   
  if (showingMode && millis() - modeDisplayTime >= 1000) {
    showingMode = false;
    lcd.clear();
  }
}

// ---------- Water Level ----------
void readWaterLevel() {
  full = (digitalRead(FLOAT_FULL) == LOW);
  half = (digitalRead(FLOAT_HALF) == LOW);
}

// ---------- Water Level LCD Text ----------
String getWaterLevelText() {
  if (full && half) return "100%";
  if (!full && half) return "UP 50%";
  return "Low";
}

// ---------- AC Sensor ----------
// ---------- Measure and print AC voltage ----------
void AC_Sensor() {
  volt_rms = get_voltage();
  Serial.print("Vrms: ");
  Serial.print(int (volt_rms));
  Serial.println(" VAC");
}

// ---------- Measure AC Voltage (RMS) ----------
int get_voltage() {
  float adc_sample;
  float volt_inst = 0;
  float sum = 0;
  int N = 0;
  unsigned long init_time = millis();

  while ((millis() - init_time) < 500) {  // Measure for 0.5 seconds (~30 cycles of 60Hz)
    adc_sample = analogRead(SENSOR_PIN);
    volt_inst = map(adc_sample, adc_min, adc_max, volt_multi_n, volt_multi_p);
    sum += sq(volt_inst); // Sum of squares
    N++;
    delay(1);
  }

  float rmsVoltage = sqrt(sum / N);         // RMS calculation
  return rmsVoltage;
}


// -------- Update voltage for lcd -------
void updateVoltage() {
  unsigned long now = millis();
  if (now - prevVoltMillis >= voltInterval) {
    prevVoltMillis = now;
    voltage = int(get_voltage());
  }
  return voltage;
}

// ---------- LCD Update ----------
void updateLCD() {
  // Check verified show or not after adjust
  // If verified is showing, hold that screen for 2s (non-blocking)
  if (Verified_showing) {
    if (millis() - verifiedStart < 2000) return;
    else Verified_showing = false;
    lcd.clear();
  }

  if (!showingMode && !adjustMode) {

  //LCD line 0

    // water flow issue found
    if(water_flow_issue){
      lcd.setCursor(0, 0);
      lcd.print("No water Flowing");
    }else if (low_voltage_problem) {
    // LCD display while in low voltage mode
    lcd.setCursor(0, 0);
    lcd.print("Low V: Wait ");
    lcd.print(safe_voltage);
    lcd.print("V   ");
    } else {
      //normal display water level
      lcd.setCursor(0, 0);
      lcd.print("W_Level : ");
      lcd.print(getWaterLevelText());
      lcd.print("   ");
    }

  //LCD line 1

    // AC voltage and save voltage(SV)
    if (currentMode == 0) {
      lcd.setCursor(0, 1);
      lcd.print("AC:");
      lcd.print(voltage);
      lcd.print("V SV:");
      lcd.print(save_voltage);
      lcd.print("V");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("AC Volt : ");
      lcd.print(voltage);
      lcd.print(" V");
    }
  }
}

// ---------- RGB Indicator ----------
void updateRGB() {
  if (!showingMode) {
    if (currentMode == 0) setColor(0, 255, 0);  //Green
    if (currentMode == 1) setColor(255, 0, 0);  //Red
    if (currentMode == 2) setColor(0, 0, 255);  //Blue
  }
}

// ---------- Buzzer ----------
void waterFullAlarm() {
  if (full && half && currentMode != 2) { //no alarm for manual mode
    if (!buzzerActive) {
      buzzerActive = true;
      buzzerStartTime = millis();
      digitalWrite(BUZZER, HIGH);
    }
    if (buzzerActive && millis() - buzzerStartTime >= 5000) {
      digitalWrite(BUZZER, LOW);
    }
  } else {
    digitalWrite(BUZZER, LOW);
    buzzerActive = false;
  }
}

// ---------- Motor Control ----------
void motorControl() {
  digitalWrite(MOTOR_LED, motorState ? HIGH : LOW);

  switch (currentMode) {

    case 0: // Save Auto Mode

      // --- Reset when returning from manual mode ---
      if (previous_mode == 2 && motorState) {
        digitalWrite(MOTOR_RELAY, LOW);
        motorState = false;
        low_voltage_problem = false;
        detect_low_volt_problem = false;
        problem_count = 0;
        Start_voltage = 0;
        Stop_voltage = 0;
        firstRunStartVoltage = 0;
        previous_mode = 0;
        Serial.println("Returned from manual mode → reset motor and flags");
      }

      else if (!low_voltage_problem) {
        safe_voltage = 0;

        // --- Voltage or full tank protection ---
        if (voltage < save_voltage || full) {
          Stop_voltage = voltage;

          if (voltage < save_voltage) {
            
            unsigned long motorRunDuration = millis() - Motar_run_time;

            // Quick stop detection (<8s)
            if (motorRunDuration < motar_QS_check_delay) {
              if (!detect_low_volt_problem) {
                detect_low_volt_problem = true;
                problem_count = millis();
                if (firstRunStartVoltage == 0) firstRunStartVoltage = Start_voltage; // preserve first motor start voltage
                Serial.println("First quick stop → 1-min monitoring started");
              } else if (millis() - problem_count <= low_V_check_delay) { // 1-min window
                low_voltage_problem = true;
                dropped_voltage = save_voltage - Stop_voltage;
                safe_voltage = Start_voltage + dropped_voltage;

                Serial.println("Low voltage problem CONFIRMED!");
                Serial.print("Motor start voltage: "); Serial.println(Start_voltage);
                Serial.print("Motor stop voltage: "); Serial.println(Stop_voltage);
                Serial.print("Safe voltage to restart: "); Serial.println(safe_voltage);
              } else {
                detect_low_volt_problem = false;
                problem_count = 0;
                firstRunStartVoltage = 0;
                Serial.println("5-min window expired → reset quick-stop detection");
              }
            }
          }

          if (motorState) {
            digitalWrite(MOTOR_RELAY, LOW);
            motorState = false;
            Motar_run_time = 0;
            water_flow_issue = false;
            Serial.println("Motor stopped due to low voltage or full tank");
          }
        }

        // Start motor if voltage safe
        else if (voltage >= save_voltage && !full && !motorState && !water_flow_issue) {
          digitalWrite(MOTOR_RELAY, HIGH);
          motorState = true;
          Motar_run_time = millis();
          if(detect_low_volt_problem){
            Start_voltage = voltage; // record current start voltage
          }
          
          if (!detect_low_volt_problem && firstRunStartVoltage == 0)
            firstRunStartVoltage = voltage;
          Serial.print("Motor started. Start voltage: "); Serial.println(voltage);
        }

        // Water flow check after 7s
        if (motorState && (millis() - Motar_run_time >= water_flow_check_delay)) {
          if (digitalRead(WATER_FLOW_DETECTION) == HIGH) {
            digitalWrite(MOTOR_RELAY, LOW);
            motorState = false;
            water_flow_issue = true;
            Motar_run_time = 0;
            Serial.println("Motor stopped due to water flow issue");
          }
        }

        // Stop if tank full
        if (full && motorState && !water_flow_issue) {
          digitalWrite(MOTOR_RELAY, LOW);
          motorState = false;
          Motar_run_time = 0;
          water_flow_issue = false;
          Serial.println("Motor stopped because tank is full");
        }
      }

      // Low voltage problem mode 
      else if (low_voltage_problem) {
        // safe voltage reached
        if (voltage >= safe_voltage) { 
          low_voltage_problem = false;
          detect_low_volt_problem = false;
          problem_count = 0;
          Start_voltage = 0;
          Stop_voltage = 0;
          firstRunStartVoltage = 0;
          safe_voltage = save_voltage; // refresh safe volt for next problem

          showSafeMsg = true;              // start showing "Safe V reached"
          safeMsgStart = millis();         // record start time
          // --- NON-BLOCKING 4 SECOND MESSAGE ---
          if (showSafeMsg) {
              lcd.setCursor(0, 0);
              lcd.print("Safe V reached   ");

              if (millis() - safeMsgStart >= 4000) {
                  showSafeMsg = false;   // stop showing message
              }
          }

          Serial.println("Voltage recovered → low voltage problem cleared, detection reset");
        }
      }

      previous_mode = 0;
      break;

    case 1: // No Save Auto Mode
      if(previous_mode == 2 && motorState){
        digitalWrite(MOTOR_RELAY, LOW);
        motorState = false;
        previous_mode = 1;
      } else if(!full && !motorState && !water_flow_issue){
        digitalWrite(MOTOR_RELAY, HIGH);
        motorState = true;
        Motar_run_time = millis();
      } else if(!full && motorState && (millis() - Motar_run_time >= water_flow_check_delay)){
        if(digitalRead(WATER_FLOW_DETECTION) == HIGH){
          digitalWrite(MOTOR_RELAY, LOW);
          motorState = false;
          water_flow_issue = true;
        }
      } else if(full && motorState && !water_flow_issue){
        digitalWrite(MOTOR_RELAY, LOW);
        motorState = false;
        Motar_run_time = 0;
        water_flow_issue = false;
      }
      previous_mode = 1;
      break;

    case 2: // Manual Mode
      digitalWrite(MOTOR_RELAY, HIGH);
      motorState = true;
      Motar_run_time = 0;
      water_flow_issue = false;
      previous_mode = 2;
      break;
  }
}

// ---------- Save AC volt Adjustment Function ----------
void handleAdjust() {
  static bool lastBtnState = HIGH;
  static unsigned long btnPressStart = 0; // press to open adjust mode
  static bool prevent_same_release = false; // wait release when first entering
  static bool verifiedShown = false; // True while showing "Verified..." (while holding)

  bool btnState = digitalRead(ADJUST_BTN);
  unsigned long now = millis();

  // ====== ENTER ADJUST MODE (Hold 3s) ======
  if (currentMode == 0 && !adjustMode && !Verified_showing) {
    if (btnState == LOW && lastBtnState == HIGH) btnPressStart = now;
    if (btnState == LOW && (now - btnPressStart >= 3000)) {
      adjustMode = true;
      prevent_same_release = true; // prevent the immediate release from exiting
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set Save V:0-230");
    }

    // Reset press timer when released before long-press threshold
    if (btnState == HIGH && lastBtnState == LOW) btnPressStart = 0;
  }

  // ====== INSIDE ADJUST MODE ======
  if (adjustMode) {
    // After entering, wait for the release that happened right after the enter long-press
    if (prevent_same_release && btnState == HIGH) prevent_same_release = false;

    
    // Show adjusting Voltage (when not showing holding-verified)
    if (!prevent_same_release && !verifiedShown) {
      new_voltage = sense_ADJUST_POT();
      lcd.setCursor(0, 1);
      lcd.print("Adj V: ");
      lcd.print(new_voltage);
      lcd.print(" V   ");
    }

    // ====== While holding >=3s show "Verified..." immediately ======
    if (!prevent_same_release && !verifiedShown) {
      if (btnState == LOW && lastBtnState == HIGH) btnPressStart = now;
      if (btnState == LOW && (now - btnPressStart >= 3000)) {
        verifiedShown = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Verified...");
      }
    }

    
    // ====== When button released after Verified (the user let go) ======
    if (verifiedShown && btnState == HIGH) {
      verifiedShown = false;
      adjustMode = false;

      // Show Saved or No Change — use the global flags so updateLCD doesn't overwrite
      lcd.clear();
      if (new_voltage != save_voltage) {
        save_voltage = new_voltage;
        EEPROM.put(0, save_voltage);
        lcd.setCursor(0, 0);
        lcd.print("Saved New V:");
        lcd.print(save_voltage);
        lcd.print(" V");
      } else {
        lcd.setCursor(0, 0);
        lcd.print("No Change");
      }

      // Start 2s verified display (use your existing globals)
      Verified_showing = true;
      verifiedStart = now;
      btnPressStart = 0;
    }

    // Reset press timer
    if (btnState == HIGH && lastBtnState == LOW && !verifiedShown) btnPressStart = 0;
  }

  lastBtnState = btnState;
}

// ---------- Potentiometer ----------
int sense_ADJUST_POT() {
  int adjust_voltage = analogRead(ADJUST_POT);
  return map(adjust_voltage, 0, 1023, 0, 250);
}

//  Finished 
//  27, NOVEMBER, 2025


