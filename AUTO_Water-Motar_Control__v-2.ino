#include <LiquidCrystal_I2C.h>

// LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// RGB
#define redPin 11
#define greenPin 10
#define bluePin 9

// MOD Switch
#define Mod_switch_sa 5   // save auto
#define Mod_switch_nsa 4  // no save auto
#define Mod_switch_mnu 3  // manual

// Float switches
#define float100Pin 12
#define float50Pin  2

// Water Full Alarm Buzzer 
#define buzzer 13

// Motar relay and led
#define motar_relay 6
#define motar_led 8

// Button to adjust save ac volt in "auto save mod"
#define adjust_button 7
#define adjust_potentiometer A1 


// Track MOD state
int currentMode = -1;
int prevMode = -1;           
unsigned long modDisplayTime = 0;  
bool showingMod = false;     

// for voltage simulation
unsigned long previousMillis = 0;  
const long interval = 500; // 500 ms interval
int voltage = 0;           

// water level sensing
bool full;
bool half;

// buzzer timing
bool buzzerActive = false;
unsigned long buzzerStartTime = 0;

// motar state
bool motar_state = false;



void setup() {
  lcd.init();
  lcd.clear();         
  lcd.backlight(); 

  // RGB Pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 

  // MOD switch PINs
  pinMode(Mod_switch_sa, INPUT_PULLUP);
  pinMode(Mod_switch_nsa, INPUT_PULLUP);
  pinMode(Mod_switch_mnu, INPUT_PULLUP);

  // Float switch pins
  pinMode(float100Pin, INPUT_PULLUP);
  pinMode(float50Pin, INPUT_PULLUP);

  // Buzzer
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  //Motar relay and led
  pinMode(motar_relay, OUTPUT);
  pinMode(motar_led, OUTPUT);
  digitalWrite(motar_relay, LOW);
  digitalWrite(motar_led, LOW);

  //Adjust button and potentiometer to adjust save ac volt in save auto mod
  

  Serial.begin(9600);
}

void loop() {
  // Read MOD states
  int save   = digitalRead(Mod_switch_sa);
  int nosave = digitalRead(Mod_switch_nsa);
  int manual = digitalRead(Mod_switch_mnu);

  currentMode = -1; // 0 = save, 1 = nosave, 2 = manual
  if (save == LOW)   currentMode = 0;
  if (nosave == LOW) currentMode = 1;
  if (manual == LOW) currentMode = 2;

  // If mode changed
  if (currentMode != prevMode && currentMode != -1) {
    prevMode = currentMode;
    modDisplayTime = millis();
    showingMod = true;

    lcd.clear();
    if (currentMode == 0) {
      setColor(0, 255, 0); // Green - save auto mod
      lcd.print("Save MOD Auto");
    } 
    else if (currentMode == 1) {
      setColor(255, 0, 0); // Red - no save auto mod
      lcd.print("NoSave MOD");
    } 
    else if (currentMode == 2) {
      setColor(0, 0, 255); // Blue - manual mod
      lcd.print("Manual MOD");
    }
  }

  // After 1000ms, return to normal display
  if (showingMod && millis() - modDisplayTime >= 1000) {
    showingMod = false;
    lcd.clear();
  }

  // Update display if not showing MOD
  if (!showingMod) {
    lcd.setCursor(0,0);   
    lcd.print("W_level: ");
    lcd.print(getWaterLevelText());
    lcd.print("   ");   // padding spaces

    lcd.setCursor(0,1);  
    lcd.print("AC Volt: ");
    lcd.print(voltage);
    lcd.print(" V   ");   
  }

  // Update simulated voltage every interval
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 
    voltage = getSimulatedVoltage();
    Serial.println(voltage);
  }


  // Keep updating RGB if mode is stable
  if (!showingMod) {
    if (currentMode == 0) setColor(0, 255, 0); // Green - save auto mod
    if (currentMode == 1) setColor(255, 0, 0); // Red - no save auto mod
    if (currentMode == 2) setColor(0, 0, 255); // Blue - manual mod
  }

  
  // Water Level Sensing
   full = digitalRead(float100Pin) == LOW;  // active LOW
   half = digitalRead(float50Pin) == LOW;   // active LOW


  // water full Alarm
  water_full_alarm();

  // Motar Mods and Main functions
  main_motor_functions();




}


// ====== Fuctions =========================================
// void fuction is only for function, not return any data


// Common cathode  RGB LED
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin,  blueValue);
}

// Function to simulate AC voltage sensor
int getSimulatedVoltage() {
  return random(160, 221); // random value between 160 and 220
}

// Function to check water level
String getWaterLevelText() {


  if (full && half) {
    return "100 %";    
  } 
  else if (!full && half) {
    return "UP 50 %";    
  } 
  else {
    return " low %";    
  }
}

void water_full_alarm(){

  if (full && half) {
    if (!buzzerActive) {  
      buzzerActive = true;
      buzzerStartTime = millis();
      digitalWrite(buzzer, HIGH);  // Turn ON buzzer
    }

      // auto stop after 5s
    if (buzzerActive && millis() - buzzerStartTime >= 5000) {
          digitalWrite(buzzer, LOW); 
    }

  }else{
    digitalWrite(buzzer, LOW); 
    buzzerActive = false;
  }
}


//================= Main Motar Functions ====================
void main_motor_functions() { 
  //motar led
  if(motar_state == true){ 
    digitalWrite(motar_led, HIGH);
  }else if(motar_state == false){
    digitalWrite(motar_led, LOW);
  }

  switch (currentMode) {
    case 0: 
      // Save Auto Mode====================
      digitalWrite(motar_led,HIGH);

      break;
    case 1:
      // NoSave Auto Mode==================
      if( !full && motar_state == false) { 
        digitalWrite(motar_relay, HIGH);
        motar_state = true;
       } else if (full && motar_state == true) {
        digitalWrite(motar_relay, LOW);
        motar_state = false;
       }
      
      break;
    case 2:
      // Manual Mode=======================
      digitalWrite(motar_relay,HIGH);
      motar_state = true;

      break;
  }
}

