#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display

//int POWER_DRAIN_PIN = A0; // configure drain speed
const int POWER_SHITCH_PIN = 2;
//int BATTERY_CHECK_PIN = 3; // do not really need it, just check i2c answer
const int OUTPUT_220V_RELAY_PIN = 6;
const int BAD_BATTERY_LED_PIN = 4;
const int GOOD_BATTERY_LED_PIN = 5;

const int32_t PERIOD_MS = 1000;

const int32_t DRAIN_VALUE_PA =     300;
const int32_t DRAIN_VALUE_MOTO =   200;
const int32_t DRAIN_VALUE_Light =  100;

const int32_t MAX_POWER = DRAIN_VALUE_PA * 60 * 120;

//------------------------ PILL / BATTERY ------------------------
const uint8_t BATTERY_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;
uint16_t BATTERY_CURSOR = 21; // for 2021 year, change it to 22 in 2022
uint16_t cursorAddress = 0;

//int32_t globalPower = 0;

void print8charsInt(int32_t x) {
  /// TODO: add cursor positioning;
  char high_digits[5] = {0, 0, 0, 0, 0};
  char low_digits[5] = {0, 0, 0, 0, 0}; 
  int32_t high_value = x / 10000;
  int32_t low_value = x % 10000;

//  Serial.print("value to show");
//  Serial.println(x);
//
//  Serial.print("h_v: ");
//  Serial.println(high_value);
//  Serial.print("l_v: ");
//  Serial.println(low_value);

  
  snprintf(high_digits, sizeof(high_digits), "%04d", high_value);
  snprintf(low_digits, sizeof(low_digits), "%04d", low_value);
  
//  Serial.print("h_d: ");
//  Serial.println(high_digits);
//  Serial.print("l_d: ");
//  Serial.println(low_digits);

  lcd.print(high_digits);
  lcd.print(low_digits);
}

void print4charsInt(int32_t x) {
  char digits[5] = {0, 0, 0, 0, 0};
  int32_t value = x % 10000;
  
  snprintf(digits, sizeof(digits), "%04d", value);
  
  lcd.print(digits);
}

void print3charsInt(int32_t x) {
  char digits[4] = {0, 0, 0, 0};
  int32_t value = x % 1000;
  
  snprintf(digits, sizeof(digits), "%03d", value);
  
  lcd.print(digits);
}

void print2charsInt(int32_t x) {
  char digits[3] = {0, 0, 0};
  int32_t value = x % 100;
  
  snprintf(digits, sizeof(digits), "%02d", value);
  
  lcd.print(digits);
}



void printRemainingPower(int32_t power) {
  lcd.setCursor(0, 0);
  lcd.print("remain: ");
  print8charsInt(power);
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
  print3charsInt(minutes);
  lcd.print("min  ");

  Serial.print("secs: ");
  Serial.println(seconds);
  print2charsInt(seconds);
  lcd.print("sec");
}

int32_t readDrainValue() {
//  int analogValue = analogRead(POWER_DRAIN_PIN);
//
//  Serial.print("analog drain value: ");
//  Serial.println(analogValue);
//  
//  int32_t drainPerStep = MIN_DRAIN_VALUE + int32_t(analogValue) * (MAX_DRAIN_VALUE - MIN_DRAIN_VALUE) / int32_t(1024);
//  
//  if (drainPerStep < PERIOD_MS / 1000)
//    drainPerStep = PERIOD_MS / 1000; // because drain per sec should be positive
//    
//  Serial.print("drain step value: ");
//  Serial.println(drainPerStep);
//
//  return drainPerStep;
  return DRAIN_VALUE_PA;
}

int32_t calcDrainPerSecond(int32_t perStep) {
  int32_t drainPerSecond = int32_t(1000) * perStep / PERIOD_MS;
  Serial.print("drain per second: ");
  Serial.println(drainPerSecond);

  return drainPerSecond;
}



int32_t readByteAsInt32() {
  int32_t rdata = 0xFF;
 
  Wire.beginTransmission(BATTERY_ADDRESS);
  //Wire.write((int)(cursorAddress >> 8));   // MSB
  Wire.write((int)(cursorAddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(BATTERY_ADDRESS, ONE_BYTE_SIZE);
 
  if (Wire.available()) {
    rdata = Wire.read();
    ++cursorAddress;
    //Serial.print("new data from I2C: ");
    //Serial.println(rdata);
  }
  
  return rdata;  
}

void writeInt32AsByte(int32_t value) {
  Serial.print("write byte value ");
  Serial.print(value);
  Serial.print(" to the address ");
  Serial.println(cursorAddress);

  //i2ceeprom.write8(cursorAddress, value);
  Wire.beginTransmission(BATTERY_ADDRESS);
  //Wire.write((int)(cursorAddress >> 8));   // MSB
  Wire.write((int)(cursorAddress & 0xFF)); // LSB
  Wire.write(value);
  Wire.endTransmission();

   ++cursorAddress;
  delay(5);
  

}

int32_t readNextInt32() {
  int32_t a = readByteAsInt32();
  int32_t b = readByteAsInt32();
  int32_t c = readByteAsInt32();
  int32_t d = readByteAsInt32();
  return a | (b << 8) | (c << 16) | (d << 24);
}

void writeNextInt32(int32_t value) {
  int32_t a = value & 0xFF;
  int32_t b = (value >> 8) & 0xFF;
  int32_t c = (value >> 16) & 0xFF;
  int32_t d = (value >> 24) & 0xFF;
  
  writeInt32AsByte(a);
  writeInt32AsByte(b);
  writeInt32AsByte(c);
  writeInt32AsByte(d);    
}

bool testBatteryMemory() {
  Wire.begin();
  Wire.beginTransmission(BATTERY_ADDRESS);
  int error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": battery OK");
    return true;
  } else {
    Serial.println(": battery not found.");
    return false;
  }
}

int32_t readBatteryValue() {
  cursorAddress = BATTERY_CURSOR;
  int32_t value = readNextInt32();
  return value;
}

void updateBatteryValue(int32_t newValue) {
  cursorAddress = BATTERY_CURSOR;
  int32_t oldValue = readNextInt32();
  if (oldValue == newValue)
    return;

  cursorAddress = BATTERY_CURSOR;
  writeNextInt32(newValue);
}



void setup() {
  Serial.begin(115200);

  pinMode(OUTPUT_220V_RELAY_PIN, OUTPUT);
  pinMode(GOOD_BATTERY_LED_PIN, OUTPUT);
  pinMode(BAD_BATTERY_LED_PIN, OUTPUT);
  pinMode(POWER_SHITCH_PIN, INPUT_PULLUP);
//  pinMode(BATTERY_CHECK_PIN, INPUT_PULLUP);

  lcd.begin(16, 2); // initialize the lcd
  // Print a message to the LCD.
  lcd.setBacklight(1);
//  globalPower = MAX_POWER;
}

void setupIndication(bool batteryOk) {
  lcd.setCursor(0, 0);
  
  if (batteryOk) {
    lcd.print("POWER    /ON");
    digitalWrite(OUTPUT_220V_RELAY_PIN, HIGH);
    digitalWrite(GOOD_BATTERY_LED_PIN, LOW);
    digitalWrite(BAD_BATTERY_LED_PIN, HIGH);
  } else {
    lcd.print("POWER OFF/");
    digitalWrite(OUTPUT_220V_RELAY_PIN, LOW);
    digitalWrite(GOOD_BATTERY_LED_PIN, HIGH);
    digitalWrite(BAD_BATTERY_LED_PIN, LOW);
    
  }
}

void loop() { 
  int32_t drainPerStep = readDrainValue();
  int32_t drainPerSecond = calcDrainPerSecond(drainPerStep);

  lcd.clear();
  lcd.setCursor(0, 0);
  
  bool batteryMemoryStatus = testBatteryMemory();
  if (not batteryMemoryStatus) {
    lcd.print("no battery #103");
    delay(PERIOD_MS);
    setupIndication(false);
    return;
  }


  /// read remaining charge from battery
  int32_t memoryPower = readBatteryValue();  
  Serial.print("power from battery: ");
  Serial.println(memoryPower);
  
  if (memoryPower < -10) {
    lcd.print("ERROR #101");
    setupIndication(false);
    delay(PERIOD_MS);
  } else if (memoryPower > MAX_POWER) {
    lcd.print("ERROR #102");
    setupIndication(false);
    delay(PERIOD_MS);    
  } else if (memoryPower > 0) {
    Serial.println("battery has some power, display");
//    printRemainingPower(memoryPower);

    int powerSwitchState = digitalRead(POWER_SHITCH_PIN);
    if (powerSwitchState == LOW) {            
      Serial.println("user-device is active, discharge battery");
      /// user-device is active
      
      setupIndication(true);

      memoryPower -= drainPerStep; 
      memoryPower = max(memoryPower, -1);
      updateBatteryValue(memoryPower);
    } else {
      Serial.println("user-device does not work");
      setupIndication(false);
      // nothing      
    }

    printRemainingTime(memoryPower, drainPerSecond);
  } else {
    setupIndication(false);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("empty battery");
  }
  
  delay(PERIOD_MS);
}
