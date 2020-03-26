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
//Serial.println(temperature());
}

void sendNumArray()
{
//  while (millis() - settleHRM < 60000)
//  {
//    heartRate();
//  }
//  Serial.println("---------BEGIN---------");
  
  char tmp[10];
  char acc[10];
  char heart[10];
  char fou[10];
  char fve[10];
  char devID[10];

//  char e[] = "<";
//  char f[] = ">";

  char chicken[60];
  dtostrf((double)temperature(), 4, 2, tmp); //try sprintf(tmp,"%4.2f", temperature());
  itoa(accelerometer(), acc, 10);
  itoa(heartRate(), heart, 10);
  itoa(functionFour(), fou, 10);
  itoa(functionFive(), fve, 10);
//  itoa(deviceID, devID, 10);

  strcpy(chicken, deviceID);
  strcat(chicken, ",");
  strcat(chicken, tmp);
  strcat(chicken, ",");
  strcat(chicken, acc);
  strcat(chicken, ",");
  strcat(chicken, heart);
  strcat(chicken, ",");
  strcat(chicken, fou);
//  strcat(chicken, f);
//  strcat(chicken, "\r\n");

  HC12.print(chicken);
  delay(100);
  HC12.print("\r\n");
//  Serial.println(chicken);
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
  float mv = (val / 1023.0) * 5000;
  float cel = mv / 10;
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
    scaledZ = mapf(rawZ, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
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
  //Also detecting a heartbeat
  if (irValue > 7000) {                                         //If a finger is detected
    Serial.println("BPM");
    Serial.println(beatAvg);

    if (checkForBeat(irValue) == true)                        //If a heart beat is detected
    {
      Serial.println("BPM");
      Serial.println(beatAvg);
      //    tone(3,1000);                                        //And tone the buzzer for a 100ms you can reduce it it will be better
      //    delay(100);
      //    noTone(3);                                          //Deactivate the buzzer to have the effect of a "bip"
      //We sensed a beat!
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
    return beatAvg;
  }
  if (irValue < 7000) {      //If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
    beatAvg = 0;
    Serial.println("Please Place ");
    Serial.println("your finger ");
  }
}

float mapf(double val, double in_min, double in_max, double out_min, double out_max)
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
