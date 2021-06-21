#include <Wire.h>

const uint8_t PILL_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;

/// different pill types
enum PlayerSignals {
TICK_SEC_SIG = 4,

RAD_RCVD_SIG,
HEAL_SIG,

TIME_TICK_1S_SIG,
TIME_TICK_10S_SIG,
TIME_TICK_1M_SIG,

PILL_ANY_SIG,
PILL_HEAL_SIG,
PILL_HEALSTATION_SIG,
PILL_BLESS_SIG,
PILL_CURSE_SIG,
PILL_ANTIRAD_SIG,
PILL_RAD_X_SIG,
PILL_GHOUL_SIG,
PILL_GHOUL_REMOVED_SIG,
PILL_REMOVED_SIG,
PILL_RESET_SIG,
PILL_TEST_SIG,

AGONY_SIG,
IMMUNE_SIG,
NOT_IMMUNE_SIG,
BLESSED_SIG,

LAST_USER_SIG,

PILL_DIAGNOSTIC = 1001
};


const int32_t PILL_TYPE_TO_CREATE = PILL_DIAGNOSTIC;
const int32_t DOSE_AFTER = 0;
const int32_t CHARGE_COUNT = 0;
const int32_t POISON_VALUE = 0;

uint16_t cursorAddress = 0;

int32_t readByteAsInt32() {
  int32_t rdata = 0xFF;
 
  Wire.beginTransmission(PILL_ADDRESS);
  //Wire.write((int)(cursorAddress >> 8));   // MSB
  Wire.write((int)(cursorAddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(PILL_ADDRESS, ONE_BYTE_SIZE);
 
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
  Wire.beginTransmission(PILL_ADDRESS);
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
  //Wire.beginTransmission(PILL_ADDRESS);  
  int32_t a = value & 0xFF;
  int32_t b = (value >> 8) & 0xFF;
  int32_t c = (value >> 16) & 0xFF;
  int32_t d = (value >> 24) & 0xFF;
  
  writeInt32AsByte(a);
  writeInt32AsByte(b);
  writeInt32AsByte(c);
  writeInt32AsByte(d);    
}

void updateNextInt32(int32_t newValue) {
  uint16_t cursorBackup = cursorAddress;
  Serial.println("try to re-write value");
  int32_t oldValue = readNextInt32();
  Serial.print("old value = ");
  Serial.println(oldValue);
  
  if (oldValue == newValue) {
    Serial.println("nothing to do, skip");
    return;
  }
    

  cursorAddress = cursorBackup;
  writeNextInt32(newValue);
}

void resetCursorToBatteryValue() {
  cursorAddress = 21; // for 2021 fallout
}

void setup() {
  Serial.begin(115200); 
  Wire.begin();
}


void loop() {
  Serial.println("write BATTERY charge value as Int32");
  resetCursorToBatteryValue();
  updateNextInt32(12000000);
    
  delay(1000);
}
