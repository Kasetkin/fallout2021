#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display

int POWER_DRAIN_PIN = A0; // configure drain speed
int POWER_SHITCH_PIN = 2;

int32_t PERIOD_MS = 1000;

int32_t MIN_DRAIN_VALUE = 100;
int32_t MAX_DRAIN_VALUE = 10000;

int32_t MAX_DRAIN_PER_SEC = MAX_DRAIN_VALUE * int32_t(1000) / PERIOD_MS;
int32_t MAX_DRAIN_PER_MINUTE = 60 * MAX_DRAIN_PER_SEC;

/// PowerArmor should work ~20 minutes, it's our max drain_value
int32_t MAX_POWER = 20 * MAX_DRAIN_PER_MINUTE;

//------------------------ PILL / BATTERY ------------------------
const uint8_t BATTERY_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;



int32_t globalPower = 0;

void print8charsInt(int32_t x) {
  /// TODO: add cursor positioning;
  char high_digits[5] = {0, 0, 0, 0, 0};
  char low_digits[5] = {0, 0, 0, 0, 0}; 
  int32_t high_value = x / 10000;
  int32_t low_value = x % 10000;

  Serial.print("value to show");
  Serial.println(x);

  Serial.print("h_v: ");
  Serial.println(high_value);
  Serial.print("l_v: ");
  Serial.println(low_value);

  
  snprintf(high_digits, sizeof(high_digits), "%04d", high_value);
  snprintf(low_digits, sizeof(low_digits), "%04d", low_value);
  
  Serial.print("h_d: ");
  Serial.println(high_digits);
  Serial.print("l_d: ");
  Serial.println(low_digits);

  lcd.print(high_digits);
  lcd.print(low_digits);
}

void print4charsInt(int32_t x) {
  /// TODO: add cursor positioning;
  char digits[5] = {0, 0, 0, 0, 0};
  int32_t value = x % 10000;

  Serial.print("4 digit value to show");
  Serial.println(x);

  Serial.print("v: ");
  Serial.println(value);
  
  snprintf(digits, sizeof(digits), "%04d", value);
  
  Serial.print("d: ");
  Serial.println(digits);

  lcd.print(digits);
}

void printRemainingPower(int32_t power) {
  lcd.setCursor(0, 0);
  lcd.print("remain: ");
  print8charsInt(globalPower);
}

void printRemainingTime(int32_t power, int32_t drainPerSecond) {
  lcd.setCursor(0, 1);

  int32_t drainPerMinute = drainPerSecond * 60;
  int32_t minutes = power / drainPerMinute;
  
  int32_t secondsPower = power % drainPerMinute;
  int32_t seconds = secondsPower / drainPerSecond;
  if (0 < (secondsPower % drainPerSecond)) 
    seconds += 1;


  Serial.print("minutes ");
  Serial.println(minutes);
  print4charsInt(minutes);
  lcd.print("min  ");

  Serial.print("secs: ");
  Serial.println(seconds);
  print4charsInt(seconds);
  lcd.print("sec");
}

int32_t readDrainValue() {
  int analogValue = analogRead(POWER_DRAIN_PIN);

  Serial.print("analog drain value: ");
  Serial.println(analogValue);
  
  int32_t drainPerStep = MIN_DRAIN_VALUE + int32_t(analogValue) * (MAX_DRAIN_VALUE - MIN_DRAIN_VALUE) / int32_t(1024);
  
  if (drainPerStep < PERIOD_MS / 1000)
    drainPerStep = PERIOD_MS / 1000; // because drain per sec should be positive
    
  Serial.print("drain step value: ");
  Serial.println(drainPerStep);

  return drainPerStep;
}

int32_t calcDrainPerSecond(int32_t perStep) {
  int32_t drainPerSecond = int32_t(1000) * perStep / PERIOD_MS;
  Serial.print("drain per second: ");
  Serial.println(drainPerSecond);

  return drainPerSecond;
}


void setup() {
  Serial.begin(115200);

  pinMode(POWER_SHITCH_PIN, INPUT_PULLUP);

  lcd.begin(16, 2); // initialize the lcd
  // Print a message to the LCD.
  lcd.setBacklight(255);
  globalPower = MAX_POWER;
}


void loop() {

  Serial.print("power : ");
  Serial.println(globalPower);
  
  int32_t drainPerStep = readDrainValue();
  int32_t drainPerSecond = calcDrainPerSecond(drainPerStep);

  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (globalPower > 0) {
    printRemainingPower(globalPower);
    printRemainingTime(globalPower, drainPerSecond);

    int powerSwitchState = digitalRead(POWER_SHITCH_PIN);
    if (powerSwitchState == LOW) {            
      globalPower -= drainPerStep; 
    } else {
      // nothing      
    }
  } else {
    globalPower = -1;
    lcd.print("empty battery");
    lcd.setCursor(3, 1);
    lcd.print("charge it");
  }
  
  delay(PERIOD_MS);
}
