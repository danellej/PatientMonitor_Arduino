int scale = 3;
boolean micro_is_5V = true;

float sittingPos = -0.30; //scaledZ
float error = 0.03;
//float lyingPos = //scaledZ

void setup()
{
  // Initialize serial communication at 115200 baud
  Serial.begin(9600);
}

void loop()
{
  // Get raw accelerometer data for each axis
  int rawX = analogRead(A0);
  int rawY = analogRead(A1);
  int rawZ = analogRead(A3);

  float scaledX, scaledY, scaledZ; // Scaled values for each axis

  if (micro_is_5V) // microcontroller runs off 5V
  {
    scaledX = mapf(rawX, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
  }
  else // microcontroller runs off 3.3V
  {
    scaledX = mapf(rawX, 0, 1023, -scale, scale);
  }

  if (micro_is_5V) // microcontroller runs off 5V
  {
    scaledY = mapf(rawY, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
  }
  else // microcontroller runs off 3.3V
  {
    scaledY = mapf(rawY, 0, 1023, -scale, scale);
  }

  if (micro_is_5V) // microcontroller runs off 5V
  {
    scaledZ = mapf(rawZ, 0, 675, -scale, scale); // 3.3/5 * 1023 =~ 675
  }
  else // microcontroller runs off 3.3V
  {
    scaledZ = mapf(rawZ, 0, 1023, -scale, scale);
  }

  // Print out raw X,Y,Z accelerometer readings
//  Serial.print("X: "); Serial.print(rawX);

  // Print out scaled X,Y,Z accelerometer readings
//  Serial.print(" X: "); Serial.print(scaledX); Serial.print(" g");

//  Serial.print("    Y: "); Serial.print(rawY);

  // Print out scaled X,Y,Z accelerometer readings
//  Serial.print("Y: "); Serial.print(scaledY); Serial.print(" g");

//  Serial.print("    Z: "); Serial.print(rawZ);

  // Print out scaled X,Y,Z accelerometer readings
//  Serial.print("Z: "); Serial.print(scaledZ); Serial.println(" g");

  delay(100);

  if (scaledZ >= sittingPos - error)
  {
    Serial.println("Sitting");
  }
  else
  {
    Serial.println("Lying down");
  }
}

float mapf(double val, double in_min, double in_max, double out_min, double out_max)
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
