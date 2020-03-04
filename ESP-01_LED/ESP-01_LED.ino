#include <SoftwareSerial.h>
SoftwareSerial esp8266(2,3); //Pin 2 & 3 of Arduino as RX and TX. Connect TX and RX of ESP8266 respectively.
#define DEBUG true
#define led_pin 11 //LED is connected to Pin 11 of Arduino


void setup()
  {
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin, LOW);
    Serial.begin(115200);
    espSetup();
  }

void loop()
  {
    if (esp8266.available())
      {
        if (esp8266.find("+IPD,"))
          {
            String msg;
            esp8266.find("?");
            msg = esp8266.readStringUntil(' ');
            String command1 = msg.substring(0, 3);
            String command2 = msg.substring(4);
                        
            if (DEBUG) 
              {
                Serial.println(command1);//Must print "led"
                Serial.println(command2);//Must print "ON" or "OFF"
              }
            delay(100);

              if (command2 == "ON") 
                    {
                      digitalWrite(led_pin, HIGH);
                    }
                   else 
                     {
                       digitalWrite(led_pin, LOW);
                     }
          }
      }
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

  void espSetup()
  {
    esp8266.begin(115200); //Baud rate for communicating with ESP8266. Your's might be different.
    Serial.println("esp begin");
    esp8266Serial("AT+RST\r\n", 5000, DEBUG); // Reset the ESP8266
    Serial.println("esp reset");
    esp8266Serial("AT+CWMODE=1\r\n", 5000, DEBUG); //Set station mode Operation
    Serial.println("esp sta mode");
//    esp8266Serial("AT+CWJAP=\"Delford-PC_Network_EXT\",\"dv8d-dy5r-pj16\"\r\n", 5000, DEBUG);//Enter your WiFi network's SSID and Password.
//    Serial.println("esp connect wifi");
    esp8266Serial("AT+CIFSR\r\n", 5000, DEBUG);//You will get the IP Address of the ESP8266 from this command. 
    Serial.println("esp ip found");
    esp8266Serial("AT+CIPMUX=1\r\n", 5000, DEBUG);
    Serial.println("esp mux 1");
    esp8266Serial("AT+CIPSERVER=1,80\r\n", 5000, DEBUG);
    Serial.println("esp server");
  }
