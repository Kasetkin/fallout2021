#include <Wire.h>

const uint8_t PILL_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;


const int32_t PILL_DIAGNOSTIC = 1001;
const int32_t PILL_RESET = 0;
const int32_t PILL_ANTIRAD = 1;
const int32_t PILL_RAD_X = 2;
const int32_t PILL_HEAL = 3;
const int32_t PILL_HEALSTATION = 4;
const int32_t PILL_BLESS = 5;
const int32_t PILL_CURSE = 6;
const int32_t PILL_GHOUL = 7;


const int32_t PILL_TYPE_TO_CREATE = PILL_GHOUL;
const int32_t VALUE_1 = 0; // DOSE_AFTER
const int32_t VALUE_2 = 0; // CHARGE_COUNT
const int32_t VALUE_3 = 0; // POISON_VALUE ?

uint16_t cursorAddress = 0;

int32_t readByteAsInt32() {
  int32_t rdata = 0xFF;
 
  Wire.beginTransmission(PILL_ADDRESS);
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

  Wire.beginTransmission(PILL_ADDRESS);
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
  int32_t oldValue = readNextInt32();
  if (oldValue == newValue) {
    Serial.println("skip");
    return;
  }
    

  cursorAddress = cursorBackup;
  writeNextInt32(newValue);
}

void resetCursorToZero() {
  cursorAddress = 0;
}

void setup() {
  Serial.begin(115200); 
  Wire.begin();
}

void loop() {
  resetCursorToZero(); 
  
  int32_t type = readNextInt32();  
  if (type == -1) {
    Serial.println("no pill");
  } else {
    Serial.print("PILL TYPE : ");
    Serial.println(type, DEC);

    Serial.println("now overwrite it with new info");
    resetCursorToZero();
    updateNextInt32(PILL_TYPE_TO_CREATE);
    updateNextInt32(VALUE_1);
    updateNextInt32(VALUE_2);
    updateNextInt32(VALUE_3);  
    delay(700);
    
    Serial.println("done; read and verify");
    
    resetCursorToZero();
    int32_t someType = readNextInt32();
    if (someType != PILL_TYPE_TO_CREATE) {
      Serial.print("verification fail, incorrect pill type ");
      Serial.println(someType);
    } else {
      Serial.println(someType);
    }

    int32_t someDose = readNextInt32();
    if (someDose != VALUE_1) {      
      Serial.print("verification fail, incorrect DoseAfter ");
      Serial.println(someDose);
    } else {
      Serial.println(someDose);
    }

    int32_t someChargeCount = readNextInt32();
    if (someChargeCount != VALUE_2) {
      Serial.print("verification fail, incorrect ChargeCnt ");
      Serial.println(someChargeCount);      
    } else {
      Serial.println(someChargeCount);      
    }

    int32_t someValue = readNextInt32();
    if (someValue != VALUE_3) {
      Serial.print("verification fail, incorrect Value ");
      Serial.println(someValue);      
    } else {
      Serial.println(someValue);
    }
  }

  delay(1000);
}
