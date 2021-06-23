#include <Wire.h>

const int32_t BATTERY_POWER_VALUE = int32_t(300) * int32_t(60) * 120;

const int GOOD_STATE_LED = 5;
const uint8_t BATTERY_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;
uint16_t BATTERY_CURSOR = 21; // for 2021 year, change it to 22 in 2022
uint16_t cursorAddress = 0;

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
  if (oldValue == newValue) {
    Serial.println("same value, skip");
    return;
  }

  cursorAddress = BATTERY_CURSOR;
  writeNextInt32(newValue);
}

void setup() {
  Serial.begin(115200); 
  Wire.begin();
  pinMode(GOOD_STATE_LED, OUTPUT);
}


void loop() {
  digitalWrite(GOOD_STATE_LED, LOW);
  
  Serial.println("test I2C memory");
  bool batteryMemoryStatus = testBatteryMemory();
  if (not batteryMemoryStatus) {
    Serial.println("no battery #103");
    delay(100);
    return;
  }

  Serial.println("write BATTERY charge value ");
  Serial.println(BATTERY_POWER_VALUE);
  updateBatteryValue(BATTERY_POWER_VALUE);
  
  digitalWrite(GOOD_STATE_LED, HIGH);
  delay(100);
}
