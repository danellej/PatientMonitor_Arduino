#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); //Pin 2 & 3 of Arduino as RX and TX. Connect TX and RX of ESP8266 respectively.
#define DEBUG true
#define led_pin 11 //LED is connected to Pin 11 of Arduino

int paramOne = 300; //value greater than 255

char message[] = "hello";
int ndx = 0;
char receivedChars[] = "";

char a = 's';

void setup()
{
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  Serial.begin(9600);
  espSetup();
}

void loop()
{
  sendArray();
  delay(100);
  receiveArray();
}

/********SEND AND RECEIVE SINGLE CHARACTERS**********/
void sendChar()
{
  esp8266.print(a);
  Serial.print("Char sent: "); Serial.println(a);
  delay(2000);
}

void rcvChar()
{
  Serial.println("Ready to receive");
  if (esp8266.available())
  {
    char rcvd = esp8266.read();
    Serial.print("Character received: "); Serial.println(rcvd);
    delay(2000);
  }
}

/********SEND AND RECEIVE STRINGS**********/
void Stringsend()
{
  String sent = "";
  for (int i = 0; i <= (sizeof(message) / sizeof(char)); i++)
  {
    char c = message[i];
    sent += c;
  }
  esp8266.print(sent);
  Serial.print("String sent: "); Serial.println(sent);

  delay(2000);
}

void rcvString()
{
  String r = "";
  // Serial.println("Ready to receive");

  while (esp8266.available() > 0) { //was while
    r = esp8266.readString();
    delay(100);
  }

  Serial.print("String received: "); Serial.println(r);
}
/********SEND AND RECEIVE INTEGERS**********/
void testIntegers() //values between 1 and 255
{
  /*write data to esp then read data stored in esp*/
  //Clear buffer
  while (esp8266.available() > 0)
  {
    esp8266.read();
  }
  Serial.println("Waiting to write");
  delay(100);
  int val = 7;
  esp8266.write(val);
  Serial.print("Value written to esp: "); Serial.println(val);
  Serial.println("Wait...");
  delay(100);

  if (esp8266.available())
  {
    int rcvdVal = esp8266.read();
    Serial.print("Value read from esp: "); Serial.println(rcvdVal);
  }
}

void testIntegers2() //values greater than 255
{
  while (esp8266.available() > 0)
  {
    esp8266.read();
  }
  Serial.println("Waiting to write");
  delay(100);
  esp8266.write(highByte(paramOne));
  esp8266.write(lowByte(paramOne));
  Serial.print("Value written to esp: "); Serial.println(paramOne);
  Serial.println("Wait...");

  delay(2000);

  if (esp8266.available())
  {
    byte rcvdValHbyte = esp8266.read();
    byte rcvdValLbyte = esp8266.read();
    int rcvdVal = (rcvdValHbyte << 8) | (rcvdValLbyte);
    Serial.print("Value read from esp: "); Serial.println(rcvdVal);
  }
}
/********SEND AND RECEIVE ARRAYS**********/
void sendArray()
{
  String c = "";
  unsigned char arr[4] = {1, 7, 11, 9};
  
  for (int i = 0; i < 4; i++)
  {
    c += arr[i];
    c+= ' ';
    delay(100);
  }
  Serial.print("Array: "); Serial.println(c);
  esp8266.print(c); //Use print with strings
}

void receiveArray()
{
  String p = "";
  if (esp8266.available())
  {
     p = esp8266.readString();
    Serial.print("Received: "); Serial.println(p);
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
