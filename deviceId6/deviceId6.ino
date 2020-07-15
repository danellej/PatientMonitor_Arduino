#include <SoftwareSerial.h>
#include <Wire.h>
#include "MAX30105.h"           //MAX3010x library
#include "heartRate.h"          //Heart rate calculating algorithm

SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

int tempPin = A2;

unsigned long settleHRM;

float maxVolt, minVolt, systol = 0, diastol = 0;
long maxVal[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long minVal[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
long maxi, mini;
long i = 0;

char deviceID[] = "6";

void heartRateSensorSetup() {
  particleSensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
}

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  heartRateSensorSetup();

  int s = analogRead(0);
  delay(10);
  randomSeed(s);

  settleHRM = millis();
}
void loop() {
  sendNumArray();
  delay(2000);
  resetArray();
//  Serial.println(temperature());
//  delay(500);
}

void sendNumArray()
{
  Serial.println("---------SETTLE---------");
  while (millis() - settleHRM < 15000)
  {
    heartRate();
  }
  Serial.println("---------BEGIN---------");

  char tmp[10];
  char acc[10];
  char heart[10];
  char fou[10];
  char fve[10];
  char devID[10];
  char maxV[10];
  char minV[10];
  char sys[10];
  char dias[10];

  //  char e[] = "<";
  //  char f[] = ">";

  char chicken[60];
  dtostrf((double)temperature(), 4, 2, tmp); //try sprintf(tmp,"%4.2f", temperature());
  itoa(accelerometer(), acc, 10);
//  itoa(heartRate(), heart, 10);
  itoa(functionFour(), fou, 10);
  itoa(functionFive(), fve, 10);
  //  itoa(deviceID, devID, 10);

  bloodPressure();
  dtostrf(maxVolt, 5, 2, maxV);
  dtostrf(minVolt, 5, 2, minV);
  dtostrf(systol, 5, 2, sys);
  dtostrf(diastol, 5, 2, dias);
  delay(10);

  int bpm = heartRate();
  itoa(bpm, heart, 10);

  strcpy(chicken, deviceID);
  strcat(chicken, ",");
  strcat(chicken, tmp);
  strcat(chicken, ",");
  strcat(chicken, acc);
  strcat(chicken, ",");
  strcat(chicken, heart);
  strcat(chicken, ",");
  strcat(chicken, maxV);
  strcat(chicken, ",");
  strcat(chicken, minV);
  strcat(chicken, ",");
  strcat(chicken, sys);
  strcat(chicken, ",");
  strcat(chicken, dias);

  HC12.print(chicken);
  delay(100);
  HC12.print("\r\n");
  Serial.println(chicken);
  //  delay(100);
}

int functionFour() {
  int randFour = random(200, 330);
  randFour = 15;
  return randFour;
}

int  functionFive() {
  int randFive = random(400, 630);
  randFive = 16;
  return randFive;
}

float temperature() {
  int  val = analogRead(tempPin);
  float mv = (val / 1023.0) * 4500;
  float cel = mv / 10;
//  delay(100);
  return cel;
}

int accelerometer() {
  int scale = 3;
  boolean micro_is_5V = true;

  float sittingPos = -0.30; //scaledZ
  float error = 0.03;

  int rawZ = analogRead(A3);

  float scaledZ; // Scaled values for each axis

  if (micro_is_5V) // microcontroller runs off 5V
  {
    scaledZ = mapf(rawZ, 0, 750, -scale, scale); // 3.3/5 * 1023 =~ 675
  }
  else // microcontroller runs off 3.3V
  {
    scaledZ = mapf(rawZ, 0, 1023, -scale, scale);
  }

  //  Serial.print("Raw Z: "); Serial.print(rawZ);

  // Print out scaled X,Y,Z accelerometer readings
  //  Serial.print("Scaled Z: "); Serial.print(scaledZ); Serial.println(" g");

  delay(100);

  if (scaledZ >= sittingPos - error)
  {
    Serial.println("Sitting");
    return 1;
  }
  else
  {
    Serial.println("Lying down");
    return 2;
  }
}

int heartRate() {
  long irValue = particleSensor.getIR();    //Reading the IR value it will permit us to know if there's a finger on the sensor or not
    if (checkForBeat(irValue) == true)                        //If a heart beat is detected
    {
      long delta = millis() - lastBeat;                   //Measure duration between two beats
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM

      if (beatsPerMinute < 255 && beatsPerMinute > 20)               //To calculate the average we strore some values (4) then do some math to calculate the average
      {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
//    return beatAvg;
  if (irValue < 50000){
    Serial.print(" No finger?");
        beatsPerMinute = 0;
    rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
  }
  return beatAvg;
}

void bloodPressure() {
  //Settle readings
  for (int s = 0; s < 20; s++) {
    particleSensor.getIR();
  }
  long arr[1000];
  unsigned long startTime = millis();
  while (millis() - startTime <= 15000) { //60000
    while (i < 15) { //4
      arr[i] = particleSensor.getIR();
      delay(1);
      //      Serial.print("arr["); Serial.print(i); Serial.print("]: "); Serial.println(arr[i]);

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
      //      Serial.print("maxVal["); Serial.print(i); Serial.print("]: "); Serial.println(maxVal[i]);
      //      Serial.print("minVal["); Serial.print(i); Serial.print("]: "); Serial.println(minVal[i]);
      i++;
    }
  }
  //Find maximum and minimum out of the 4
  for (int q = 0; q < 15; q++) {
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

  //  Serial.print("Maximum voltage: "); Serial.print(maxVolt);
  //  Serial.print("\tMinimum voltage: "); Serial.println(minVolt);
  //  Serial.print("Systole: "); Serial.println(systol);
  //  Serial.print("Diastole: "); Serial.println(diastol);
}

void resetArray()
{
  for (int q = 0; q < 15; q++) {
    maxVal[i] = 0;
    minVal[i] = 0;
  }
}

float mapf(double val, double in_min, double in_max, double out_min, double out_max)
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
