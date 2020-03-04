#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); //Pin 2 & 3 of Arduino as RX and TX. Connect TX and RX of ESP8266 respectively.
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

#define DEBUG true

int ndx = 0;

char ssid[] = "\"Delford-PC-Network_EXT\"";
char password[] = "\"dv8d-dy5r-pj16\"";

char data[50];

//int arr[4];

//const byte numChars = 32;
//char receivedChars[numChars];

//boolean newData = false;

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
//  Serial.println("Start");
  delay(3000);
  espSetup();
  delay(3000);
}

void loop() {
  receiveNumArray();
//  HC12.listen();
//  
//  while (HC12.available() > 0) {
//    readline(data, 150, 0);
//  }
//  
//  Serial.print("bData: "); Serial.println(data);
//  HC12.end();
//  Serial.print("aData: "); Serial.println(data);
//  delay(10000);
//  esp8266.listen();
//  delay(10);
//  updateServer(data);
//  esp8266.end();
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
  int n = 0;
  HC12.listen();
  char val[30];
  while (HC12.available() > 0) { //was while
    //strcpy(val, HC12.readString()); //put things into empty character array
    char data[50];
    readline(data, 150, 0);
    Serial.println("START");
    Serial.println(data);
    Serial.println("STOP");
    //    Serial.print(p);

    //  }
    //  if (fin == true)
    //  {
    //    Serial.println("End HC12, listen to ESP");
    //    //ndx = 0;
    //
    //    Serial.print("n: "); Serial.println(n);
    //
    HC12.end();
    delay(2000);
    //    //
        esp8266.listen();
        delay(2000);
        Serial.println("--------------UPDATE SERVER--------------");
        updateServer(data);
        Serial.println("Server updated");
    //    esp8266.end();
  }
}

void receiveString()
{

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
  Serial.println("ESP receiving");
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
  delay(10000);
  esp8266Serial("AT+CWMODE=1\r\n", 5000, DEBUG); //Set station mode Operation
  delay(10000);
  Serial.println("esp sta mode");
  String cwjap = "";
  cwjap += "AT+CWJAP=";
  cwjap += ssid;
  cwjap += ",";
  cwjap += password;
  cwjap += "\"\r\n";
  esp8266Serial(cwjap, 5000, DEBUG);
//  esp8266Serial("AT+CWJAP=\"MW41NF_6C08\",\"00830131\"\r\n", 5000, DEBUG);//Enter your WiFi network's SSID and Password.
//esp8266Serial("AT+CWJAP=\"MonaConnect\",\"\"\r\n", 5000, DEBUG);//Enter your WiFi network's SSID and Password.
  //  esp8266Serial("AT+CWJAP=\"JARS\",\"008273435\"\r\n", 5000, DEBUG);//Enter your WiFi network's SSID and Password.
  delay(5000);
  Serial.println("esp connect wifi");
  esp8266Serial("AT+CIFSR\r\n", 5000, DEBUG);//You will get the IP Address of the ESP8266 from this command.
  delay(5000);
  Serial.println("esp ip found");
//  esp8266Serial("AT+CIPMUX=0\r\n", 5000, DEBUG);
//  delay(10000);
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

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout)
{
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true)
  {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while (HC12.available())
    {
      char c =  HC12.read();
      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;

      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;

        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }

    if (timeoutvalid && timeout == 0)
    {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}

void updateServer(char* data)
{
  char body[200];
  strcpy(body, "{\"data\":\"");
  strcat(body, data);
  //  strcpy(body, "\",");
  //  strcpy(body, "\"name\":\"");
  //  strcat(body, data);
  //  strcpy(body, "\",");
  //  strcpy(body, "\"age\":\"");
  //  strcat(body, data);
  strcat(body, "\"}");
  //  String body = "{\"patientId\":\"r4333\", \"name\":\"harry\", \"age\":\"76\"}";
  //  Serial.print("Body: "); Serial.println(body);
  int bodySize = strlen(body);
  //  int bodySize = body.length();
  char bodySizeString[3];
  itoa(bodySize, bodySizeString , 10);

  Serial.print("Body: "); Serial.println(body);
  char request[200];
  //  strcpy(request, "POST / HTTP/1.1\r\nHost: philliplogan.com\r\nContent-Type: application/json\r\nContent-Length: "); //route after post /
  //  strcpy(request, "POST /patient\r\nHost: 192.168.1.108:3000\r\nContent-Type: application/json\r\nContent-Length: "); //route after post /
  strcpy(request, "POST /patient/parse\r\nHost: 192.168.1.122:3000\r\nContent-Type: application/json\r\nContent-Length: "); //route after post /
  strcat(request, bodySizeString);
  strcat(request, "\r\n\r\n");
  strcat(request, body);
  strcat(request, "\r\n\r\n");

  int requestLength = strlen(request);
  char requestLengthStr[3];
  itoa(requestLength, requestLengthStr, 10);

  char cipsend[50];
  strcpy(cipsend, "AT+CIPSEND=");
  strcat(cipsend, requestLengthStr);
  strcat(cipsend, "\r\n");

  esp8266Serial("AT+CIPSTART=\"TCP\",\"192.168.1.122\",3000\r\n", 5000, DEBUG);
  delay(5000);

  esp8266Serial(cipsend, 1000, DEBUG);

  esp8266Serial(request, 1000, DEBUG);
}
