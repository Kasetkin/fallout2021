#include <Wire.h>

const uint8_t PILL_ADDRESS = 80;
const uint8_t ONE_BYTE_SIZE = 1;

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
  }
  
  return rdata;  
}


int32_t readNextInt32() {
  int32_t a = readByteAsInt32();
  int32_t b = readByteAsInt32();
  int32_t c = readByteAsInt32();
  int32_t d = readByteAsInt32();
  return a | (b << 8) | (c << 16) | (d << 24);
}

void resetCursorToZero() {
  cursorAddress = 0;
}

void setup() {
  Serial.begin(115200); 
  Wire.begin();
}

bool pillConnected = false;

void loop() {
  resetCursorToZero(); 
  int32_t type = readNextInt32();
  
  if (pillConnected == true && type == -1){
    // if pill was connected and now it's not
    
    pillConnected = false;
    Serial.println();
    Serial.println("No pill connected.");
    
  } else if (pillConnected == false && type != -1) {
    // if pill wasn't connected and now it is
    
    pillConnected = true;
    
    resetCursorToZero();
    
    int32_t someType = readNextInt32();
    Serial.println();
    Serial.print("TYPE: ");
    Serial.println(someType);

    int32_t someDose = readNextInt32();
    Serial.print("DOSE AFTER: ");
    Serial.println(someDose);

    int32_t someChargeCount = readNextInt32();
    Serial.print("CHARGE COUNT: ");
    Serial.println(someChargeCount);

    int32_t someValue = readNextInt32();
    Serial.print("VALUE: ");
    Serial.println(someValue);
    
    Serial.println("CSV copy-paste:");
    Serial.print(someType);
    Serial.print(",NO COLOR,"); // pill color, manual entry
    Serial.print(someDose);
    Serial.print(",");
    Serial.print(someChargeCount);
    Serial.print(",");
    Serial.print(someValue);
    Serial.println(",");
  } else {
    Serial.print("#");
  }

  delay(1000);
}
