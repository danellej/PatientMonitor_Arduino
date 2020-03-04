#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); //Pin 2 & 3 of Arduino as RX and TX. Connect TX and RX of ESP8266 respectively.
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

#define DEBUG true

int ndx = 0;

//char ssid[] = "\"MW41NF_6C08\"";
//char password[] = "\"00830131\"";
//
//char server[] = "192.168.1.108";

char ssid[] = "\"Delford-PC_Network_EXT\"";
char password[] = "\"dv8d-dy5r-pj16\"";

char server[] = "192.168.1.135";

char port[] = "3000";

char data[50];

int setupLED = 8;
int serverLED = A5;

void setup() {
  pinMode(setupLED, OUTPUT);
  pinMode(serverLED, OUTPUT);
  digitalWrite(setupLED, HIGH);
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  delay(3000);
  Serial.println("here");
  espSetup();
  delay(3000);
  digitalWrite(setupLED, LOW);
}

void loop() {
  receiveNumArray();
}

void receiveNumArray()
{
  HC12.listen();
  char val[30];
  while (HC12.available() > 0) { //was while
    char data[50];
//    readline(data, 150, 0);
    readline(data, 5, 0);
    Serial.println("START");
    Serial.println(data);
    Serial.println("STOP");

    HC12.end();
    delay(5000);
    
    esp8266.listen();
    delay(2000);
    Serial.println("--------------UPDATE SERVER--------------");
    updateServer(data);
    analogWrite(serverLED, 255);
    Serial.println("Server updated");
    analogWrite(serverLED, 0);
    Serial.println("Done"); 
    while(1);
//    esp8266.end();
//    delay(2000);
  }
}

/********SETUP ESP WITH AT COMMANDS**********/
void espSetup()
{
  esp8266.begin(9600); //Baud rate for communicating with ESP8266. Your's might be different.
  esp8266Serial("AT+RST\r\n", 5000, DEBUG); // Reset the ESP8266
  delay(10000);
  esp8266Serial("AT+CWMODE=1\r\n", 5000, DEBUG); //Set station mode Operation
  delay(10000);
  String cwjap = "";
  cwjap += "AT+CWJAP=";
  cwjap += ssid;
  cwjap += ",";
  cwjap += password;
  cwjap += "\r\n";
  esp8266Serial(cwjap, 5000, DEBUG);
  delay(5000);
  esp8266Serial("AT+CIFSR\r\n", 5000, DEBUG);//You will get the IP Address of the ESP8266 from this command.
  delay(5000);
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
  strcat(body, "\"}");
  int bodySize = strlen(body);
  char bodySizeString[3];
  itoa(bodySize, bodySizeString , 10);

  Serial.print("Body: "); Serial.println(body);
  char request[200];
  strcpy(request, "POST /patient/parse\r\nHost: "); //route after post /
  strcat(request, server);
  strcat(request, ":");
  strcat(request, port);
  strcat(request, "\r\nContent-Type: application/json\r\nContent-Length: ");
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

  char cipstart[100];
  strcpy(cipstart, "AT+CIPSTART=\"TCP\",\"");
  strcat(cipstart, server);
  strcat(cipstart, "\",");
  strcat(cipstart, port);
  strcat(cipstart, "\r\n");
  esp8266Serial(cipstart, 5000, DEBUG);
  delay(5000);

  esp8266Serial(cipsend, 1000, DEBUG);

  esp8266Serial(request, 1000, DEBUG);
}
