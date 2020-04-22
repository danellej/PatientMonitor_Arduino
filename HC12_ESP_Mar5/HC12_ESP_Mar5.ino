#include <SoftwareSerial.h>

SoftwareSerial HC12SS = SoftwareSerial(10, 11);
SoftwareSerial *HC12 = &HC12SS;

SoftwareSerial espSS = SoftwareSerial(2, 3);
SoftwareSerial *esp = &espSS;

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

#define DEBUG true

char ssid[] = "\"Delford-PC_Network\"";
char password[] = "\"dv8d-dy5r-pj16\"";

char server[] = "192.168.1.106";

char port[] = "3000";

void setup() {
  Serial.begin(9600);
  espSetup();
}

void loop() {
  char line[50];

  readline(line, 50);

//  updateServer(line);
Serial.println(line);
}

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout)
{
  Serial.println(F("READLINE"));
  HC12->begin(9600);
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true)
  {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while (HC12->available())
    {
      //      Serial.println("c=HC12.read()");
      char c =  HC12->read();
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
      Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  HC12->end();
  return buffidx;
}

void updateServer(char* data)
{
  esp->begin(9600);
  char body[70];
  strcpy(body, "{\"data\":\"");
  strcat(body, data);
  strcat(body, "\"}");
  int bodySize = strlen(body);
  char bodySizeString[3];
  itoa(bodySize, bodySizeString , 10);

  Serial.print(F("Body: ")); Serial.println(body);
  char request[160];
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
  char requestLengthStr[4];
  itoa(requestLength, requestLengthStr, 10);

  char cipsend[50];
  strcpy(cipsend, "AT+CIPSEND=");
  strcat(cipsend, requestLengthStr);
  strcat(cipsend, "\r\n");

  char cipstart[40];
  strcpy(cipstart, "AT+CIPSTART=\"TCP\",\"");
  strcat(cipstart, server);
  strcat(cipstart, "\",");
  strcat(cipstart, port);
  strcat(cipstart, "\r\n");
  Serial.println("-----CIPSTART--------");
  esp8266Serial(cipstart, 1000, DEBUG);
  delay(500);

  Serial.println("-----CIPSEND--------");
  Serial.println(cipsend);
  esp8266Serial(cipsend, 200, DEBUG);
  delay(100);

  Serial.println("-----REQUEST--------");
  esp8266Serial(request, 10000, DEBUG);
  delay(100);
  esp->end();
}

void espSetup()
{
  Serial.println(F("INITIALIZE ESP"));
  esp->begin(9600); //Baud rate for communicating with esp. Your's might be different.
  esp8266Serial("AT+RST\r\n", 5000, DEBUG); // Reset the esp
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
  esp8266Serial("AT+CIFSR\r\n", 5000, DEBUG);//You will get the IP Address of the esp from this command.
  delay(5000);
  esp->end();
  Serial.println(F("END INITIALIZATION"));
}

String esp8266Serial(String command, const int timeout, boolean debug)
{
  String response = "";
  esp->print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp->available())
    {
      char c = esp->read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
    if(response.indexOf("+IPD") == 31){
      Serial.println("---------RESPONSE RECEIVED----------");
    }
  }
  return response;
}

void checkResponse(String a) {
  if ((a.indexOf("+IPD") != -1)){
    delay(1000);
    Serial.println("---------RESPONSE RECEIVED----------");
  }
}
