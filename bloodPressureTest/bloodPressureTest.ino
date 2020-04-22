#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

long maxVal[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long minVal[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float maxVolt, minVolt, systol = 0, diastol = 0;
long maxi, mini;
long i = 0;

void setup()
{
  Serial.begin(9600);
  // Initialize sensor
  if (particleSensor.begin() == false)
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
  int s = analogRead(0);
  delay(10);
  randomSeed(s);
}

void loop()
{
  getReadings();
  findExtremes();
  while (1);
}

void bloodPressure() { //not working
  //  long maxi, mini;
  //  long maxVal[4] = {0, 0, 0, 0};
  //  long minVal[4] = {0, 0, 0, 0};
  //  float maxVolt, minVolt, systol = 0, diastol = 0;



  //Settle readings

  //
  for (int p = 0; p < 4; p++) {
    Serial.print("p: "); Serial.println(p);
    getReadings();
    p++;
    i = 0;
  }
}

void getReadings()
{
  for (int s = 0; s < 20; s++) {
    particleSensor.getIR();
  }
  long arr[1000];
  unsigned long startTime = millis();
  while (millis() - startTime <= 15000) { //60000
    while (i < 15) { //4
      arr[i] = particleSensor.getIR();
      delay(1);
      Serial.print("arr[");Serial.print(i);Serial.print("]: ");Serial.println(arr[i]);

      if (i == 0) {
        maxVal[i] = arr[0];
        minVal[i] = arr[0];
      }
      if (i > 0) {
        if (arr[i] > maxVal[i - 1]) {
          maxVal[i] = arr[i];
        } else maxVal[i] = maxVal[i - 1];
        if (arr[i] < minVal[i - 1]) {
          minVal[i] = arr[i];
        } else minVal[i] = minVal[i - 1];
      }
      Serial.print("maxVal[");Serial.print(i);Serial.print("]: ");Serial.println(maxVal[i]);
      Serial.print("minVal[");Serial.print(i);Serial.print("]: ");Serial.println(minVal[i]);
      i++;
    }
  }
}

void findExtremes() {
  //Find maximum and minimum out of the 4
  for (int q = 0; q < 15; q++) {
//    Serial.print("Maximum value"); Serial.print("["); Serial.print(q); Serial.print("]: "); Serial.print(maxVal[q]);
//    Serial.print("\tMinimum value: "); Serial.print("["); Serial.print(q); Serial.print("]: "); Serial.print(minVal[q]);
//    Serial.println();

    if (q == 0) {
      maxi = maxVal[q]; //maxVal [q-1] = maxVal[q]
      mini = minVal[q]; //maxVal [q-1] = maxVal[q]
    }
    if (q > 0 ) {
      if (maxVal[q] > maxVal[q - 1]) {
        maxi = maxVal[q];
      }
      else if (minVal[q] < minVal[q - 1]) {
        mini = minVal[q];
      }
    }

    systol += maxVal[q];
    diastol += minVal[q];
  }

  maxVolt = (maxi * (1.8 / 1023)); //262144
  minVolt = (mini * (1.8 / 1023)); //262144
  systol /= 15; systol = (systol * (1.8 / 1023));
  diastol /= 15; diastol = (diastol * (1.8 / 1023));

  Serial.print("Maximum voltage: "); Serial.print(maxVolt);
  Serial.print("\tMinimum voltage: "); Serial.println(minVolt);
  Serial.print("Systole: "); Serial.println(systol);
  Serial.print("Diastole: "); Serial.println(diastol);
}

int randNum() {
  int randOne = random(30, 80);
  return randOne;
}

void getAll() {
  Serial.print(" R[");
  Serial.print(particleSensor.getRed());
  Serial.print("] IR[");
  Serial.print(particleSensor.getIR());
  Serial.print("] G[");
  Serial.print(particleSensor.getGreen());
  Serial.print("]");

  Serial.println();
}
