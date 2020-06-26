#include <math.h>

int val;
int tempPin = 2;

char data[100];
float myFloat = 99.9911;

void setup() {
  Serial.begin(9600);
}

void loop() {
  val = analogRead(tempPin);
  float mv = (val/1023.0)*5000;
  float cel = mv/10;
  Serial.print("Temp: "); Serial.println(cel);
  delay(100);

//  dtostrf(cel, 4, 2, data);
//  Serial.print("String version: "); Serial.println(data);
}
