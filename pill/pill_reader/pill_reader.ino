#include <Wire.h>

void setup() {
  Wire.begin();

  Serial.begin(9600);
  while (!Serial); // Leonardo: wait for serial monitor
  Serial.println("I2C Pill Reader");
}

const int PILL_ADDRESS = 80;

/// different pill types
const int PILL_ENUMERATOR = 1;
const int PILL_HEAL_POISON = 9;
const int PILL_DRUG = 10;

int32_t readByteAsInt(char address) {
  Wire.requestFrom(address, 1);

  if (Wire.available())
    return Wire.read();
  else
    return -1;  
}


int32_t readInt(char address) {

  int32_t a = readByteAsInt(address);
  int32_t b = readByteAsInt(address);
  int32_t c = readByteAsInt(address);
  int32_t d = readByteAsInt(address);

//  Serial.print("a ");
//  Serial.println(a);
//  Serial.print("b ");
//  Serial.println(b);
//  Serial.print("c ");
//  Serial.println(c);
//  Serial.print("d ");
//  Serial.println(d);

  
  return a | (b << 8) | (c << 16) | (d << 24);
}

void loop() {
  Serial.println("-----------------");

  /// reset address from where we read data
  Wire.beginTransmission(PILL_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  
  int32_t type = readInt(PILL_ADDRESS);  
  if (type == -1) {
    Serial.println("no pill");
  } else {
    Serial.print("PILL TYPE : ");
    Serial.println(type, DEC);
  }
  
//  switch (type) {
//  }
//  Serial.print("field 1 = ");
//  Serial.println(fieldA, DEC);
//  Serial.print("field 2 = ");
//  Serial.println(fieldB, DEC);

  delay(1000); // Wait 5 seconds for next scan
}
