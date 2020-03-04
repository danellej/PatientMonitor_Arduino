#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

char deviceID[] = "9";

int scale = 3;
boolean micro_is_5V = true;

float sittingPos = -0.30; //scaledZ
float error = 0.03;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
}

void loop() {
  sendNum();
}

void sendNum()
{
  char acc[10];

  char sendNums[50];
  itoa(accelerometer(), acc, 10);

  strcpy(sendNums, deviceID);
  strcat(sendNums, ",");
  strcat(sendNums, acc);

  HC12.print(sendNums);
  Serial.println(sendNums);
}

int accelerometer()
{
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

float mapf(double val, double in_min, double in_max, double out_min, double out_max)
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
