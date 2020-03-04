#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); //Pin 2 & 3 of Arduino as RX and TX. Connect TX and RX of ESP8266 respectively.
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
SoftwareSerial *hc = &HC12;

#define DEBUG true

int setupLED = 8;

char ssid[] = "\"MW41NF_6C08\"";
char password[] = "\"00830131\"";

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout);

void setup() {
  Serial.begin(9600);
  pinMode(setupLED, OUTPUT);
  digitalWrite(setupLED, HIGH);
//  hc->begin(9600);
//  esp8266.begin(9600);

  Serial.println("-----SETUP------");
  digitalWrite(setupLED, LOW);
}

void loop() {
  Serial.println("Start");
  
  char data[50];
  readline(data, 150, 0);
  Serial.print("bData: "); Serial.println(data);
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
  hc->begin(9600);
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;

  while (true)
  {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while (hc->available())
    {
      char c =  hc->read();
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
  hc->end();
  return buffidx;
}
