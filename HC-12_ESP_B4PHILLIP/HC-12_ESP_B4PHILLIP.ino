#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); //Pin 2 & 3 of Arduino as RX and TX. Connect TX and RX of ESP8266 respectively.
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

#define DEBUG true

int ndx = 0;

//int arr[4];

//const byte numChars = 32;
//char receivedChars[numChars];

//boolean newData = false;

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  delay(3000);
  espSetup();
  delay(3000);
}

void loop() {
  //  HC12receiveArray();
  receiveNumArray();
}

/********HC-12 RECEIVE AND WRITE TO ESP**********/
/*void HC12receiveArray()
{
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  // if (Serial.available() > 0) {
  HC12.listen();
  while (HC12.available() > 0 && newData == false) {
    rc = HC12.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        //  receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        //    receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
  if (newData == true)
  {
    HC12.end();
    esp8266.listen();
    //    esp8266.print(receivedChars);
    delay(1000);
    //    Serial.print("Written to ESP: "); Serial.println(receivedChars);
    newData = false;
    ESPreceiveArray();
  }
} */

void receiveNumArray()
{
  boolean fin = false;
  int receivedChars[] = {0, 0, 0, 0};
  //  int ndx = 0;
  HC12.listen();
  while (HC12.available() > 0 && fin == false) { //was while
    int val = HC12.read();
    if (val != 0)
    {
      receivedChars[ndx] = val;
      Serial.println(receivedChars[ndx]);
      ndx++;
      delay(100);
    }
    else
    {
      fin = true;
    }
  }
  if (fin == true)
  {
    Serial.println("End HC12, listen to ESP");
    byte arraySize = ndx;
    ndx = 0;
    Serial.print("Array size: "); Serial.println(arraySize);
    Serial.print("ndx: "); Serial.println(ndx);

    //    delay(10);
    HC12.end();
    //    esp8266.listen();
    //    esp8266.print("READY");
    delay(1); //5
    esp8266.listen();
    // esp8266.print("READY");
    //delay(10);
    //ESPreceiveArray();
    //    delay(10);

    for (int i = 0; i < arraySize; i++)
    {
      Serial.print("Received chars: "); Serial.println(receivedChars[i]);
//      esp8266.write(receivedChars[i]);
      //      esp8266.print('a');
      delay(10);
//      Serial.print("Value read from esp: "); Serial.println(esp8266.read());
    }
    delay(1000);
    //    if (esp8266.available()>0)
    //    {
    //      Serial.println("ESP Available");
    //      int rcvdVal = esp8266.read();
    //      Serial.print("Value read from esp: "); Serial.println(rcvdVal);
    //    }

    fin = false;
  }
}
/********SEND ARRAY FROM HC12 TO ESP**********/
//void ESPsendArray()
//{
//  String c = "";
//  unsigned char arr[4] = {1, 7, 11, 9};
//
//  for (int i = 0; i < 4; i++)
//  {
//    c += arr[i];
//    c += ' ';
//    delay(100);
//  }
//  Serial.print("Array: "); Serial.println(c);
//  esp8266.print(c); //Use print with strings
//}
//
///********CONFIRM WHAT ESP RECEIVED**********/
void ESPreceiveArray()
{
  String p = "";
  if (esp8266.available() > 0)
  {
    p = esp8266.readString(); //read returns number
    Serial.print("ESP Received: "); Serial.println(p);
  }
}

/********SETUP ESP WITH AT COMMANDS**********/
void espSetup()
{
  esp8266.begin(9600); //Baud rate for communicating with ESP8266. Your's might be different.
  Serial.println("esp begin");
  esp8266Serial("AT+RST\r\n", 5000, DEBUG); // Reset the ESP8266
  Serial.println("esp reset");
  esp8266Serial("AT+CWMODE=1\r\n", 5000, DEBUG); //Set station mode Operation
  Serial.println("esp sta mode");
  //esp8266Serial("AT+CWJAP=\"Delford-PC_Network_EXT\",\"dv8d-dy5r-pj16\"\r\n", 5000, DEBUG);//Enter your WiFi network's SSID and Password.
  //Serial.println("esp connect wifi");
  esp8266Serial("AT+CIFSR\r\n", 5000, DEBUG);//You will get the IP Address of the ESP8266 from this command.
  Serial.println("esp ip found");
  esp8266Serial("AT+CIPMUX=1\r\n", 5000, DEBUG);
  Serial.println("esp mux 1");
  esp8266Serial("AT+CIPSERVER=1,80\r\n", 5000, DEBUG);
  Serial.println("esp server");
}

String esp8266Serial(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
