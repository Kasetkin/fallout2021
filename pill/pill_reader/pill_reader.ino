#include <Wire.h>

void setup() {
  Wire.begin();

  Serial.begin(9600);
  while (!Serial); // Leonardo: wait for serial monitor
  Serial.println("I2C Pill Reader");
}

const int PILL_ADDRESS = 80;

int32_t readInt(char address) {
  Wire.requestFrom(address, 4);

//  while (Wire.available()) { // slave may send less than requested
//    Serial.print("avaliable bytes at address ");
//    Serial.println(address, DEC);
//    char c = Wire.read(); // receive a byte as character
//    Serial.print(c);         // print the character
//  }
  int32_t a = 0;
  int32_t b = 0;
  int32_t c = 0;
  int32_t d = 0;
  if (Wire.available())
    a = Wire.read();
  else
    return 0;
     
  if (Wire.available())
    b = Wire.read();
  else
    return 0;
    
  if (Wire.available())
    c = Wire.read();
  else 
    return 0;
    
  if (Wire.available())
    d = Wire.read();
  else
    return 0;

  Serial.print("a ");
  Serial.println(a);
  Serial.print("b ");
  Serial.println(b);
  Serial.print("c ");
  Serial.println(c);
  Serial.print("d ");
  Serial.println(d);
  
    
  //return (a << 24) | (b << 16) | (c << 8) | d;
  return a | (b << 8) | (c << 16) | (d << 24);
}

void loop() {
//  for (byte address = 0; address < 127; ++address) {
//    readInt(address);
//  }
  Serial.println("-----------------");
  Wire.beginTransmission(PILL_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  
  int32_t type = readInt(PILL_ADDRESS);
  int32_t fieldA = readInt(PILL_ADDRESS);
  int32_t fieldB = readInt(PILL_ADDRESS);

  Serial.print("PILL TYPE : ");
  Serial.println(type, DEC);
  Serial.print("field 1 = ");
  Serial.println(fieldA, DEC);
  Serial.print("field 2 = ");
  Serial.println(fieldB, DEC);

  delay(5000); // Wait 5 seconds for next scan
}
