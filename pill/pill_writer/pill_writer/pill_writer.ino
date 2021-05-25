#include <Wire.h>

const uint8_t PILL_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;

/// different pill types
const int PILL_ENUMERATOR = 1;
const int PILL_HEAL_POISON = 9;
const int PILL_DRUG = 10;


const int32_t PILL_TYPE_TO_CREATE = 12;
const int32_t DOSE_AFTER = 0;
const int32_t CHARGE_COUNT = 100;
const int32_t POISON_VALUE = -7200;

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

void resetCursorToZero() {
  cursorAddress = 0;
}

//int32_t toWrite = 0;

void setup() {
  Serial.begin(115200); 
  Wire.begin();
//  toWrite = 111111111;
}


//void loop0() {
//  Serial.println("-----------------");
//  
////  if (i2ceeprom.begin(PILL_ADDRESS)) {
////    Serial.println("found PILL");
////  } else {
////    Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
////    delay(1000);    
////    return;
////  }  
//
//  resetCursorToZero();
//  int32_t type = readNextInt32();
//  Serial.print("PILL TYPE : ");
//  Serial.println(type, DEC);
//
//  Serial.print("try to write new value ");
//  Serial.println(toWrite);
//  resetCursorToZero();
//  writeNextInt32(toWrite);
//
//  ++toWrite;
//  delay(5000);
//}

void loop() {
  resetCursorToZero(); 
  int32_t type = readNextInt32();  
  if (type == -1) {
    Serial.println("no pill");
  } else {
    Serial.print("PILL TYPE : ");
    Serial.println(type, DEC);

//    Serial.println("now overwrite it with new info");
//    resetCursorToZero();
//    writeNextInt32(PILL_TYPE_TO_CREATE);
//    writeNextInt32(DOSE_AFTER);
//    writeNextInt32(CHARGE_COUNT);
//    writeNextInt32(POISON_VALUE);  
//    delay(5000);
//    
//    Serial.println("done; read and verify");
    
    resetCursorToZero();
    int32_t someType = readNextInt32();
    if (someType != PILL_TYPE_TO_CREATE) {
      Serial.print("verification fail, incorrect pill type ");
      Serial.println(someType);
    } else {
      Serial.println(someType);
    }

    int32_t someDose = readNextInt32();
    if (someDose != DOSE_AFTER) {      
      Serial.print("verification fail, incorrect DoseAfter ");
      Serial.println(someDose);
    } else {
      Serial.println(someDose);
    }

    int32_t someChargeCount = readNextInt32();
    if (someChargeCount != CHARGE_COUNT) {
      Serial.print("verification fail, incorrect ChargeCnt ");
      Serial.println(someChargeCount);      
    } else {
      Serial.println(someChargeCount);      
    }

    int32_t someValue = readNextInt32();
    if (someValue != POISON_VALUE) {
      Serial.print("verification fail, incorrect Value ");
      Serial.println(someValue);      
    } else {
      Serial.println(someValue);
    }
  }

  delay(1000);
}
