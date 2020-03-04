/* 1byte HC12 TRANSMITTER example.
  /* Tutorial link: http://electronoobs.com/eng_arduino_tut96.php
   Code: http://electronoobs.com/eng_arduino_tut96_code1.php
   Scheamtic: http://electronoobs.com/eng_arduino_tut96_sch1.php
   Youtube Channel: http://www.youtube/c/electronoobs

  Module // Arduino UNO/NANO
    GND    ->   GND
    Vcc    ->   3.3V
    Tx     ->   D10
    Rx     ->   D11
*/
#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin

int button1 = 6;
int button2 = 7;
int button3 = 8;

int val = 0;

unsigned char arr[] = {7, 8, 9, 10};

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  //  pinMode(button1, INPUT_PULLUP);
  //  pinMode(button2, INPUT_PULLUP);
  //  pinMode(button3, INPUT_PULLUP);
}
void loop() {
//   char doop[] = "12,13,14,15,16";
   char doop2[] = "17,18,19,20,21";
//   HC12.println(doop);
   HC12.println(doop2);
   delay(1000);
}

void ledButtons() {
  int button1State = digitalRead(button1);
  int button2State = digitalRead(button2);
  int button3State = digitalRead(button3);

  Serial.print("Val: "); Serial.println(val);

  if (button1State == 0) {
    val = 1;
  }
  if (button2State == 0) {
    val = 2;
  }
  if (button3State == 0) {
    val = 0;
  }
  //  int lastVal = val;
  HC12.write(val);      // Send that data to HC-12
}

void sendNumArrayNO()
{
  for (int i = 0; i <= (sizeof(arr) / sizeof(char)); i++) {
    if (i != (sizeof(arr) / sizeof(char)))
    {
      HC12.write(arr[i]);
      Serial.println(arr[i]);
      delay(100);
    }
    else //send end character
    {
      int num = 0;
      HC12.write(num);
      Serial.println(0);
    }
  }
}

void sendNumArray()
{
  for (int i = 0; i < (sizeof(arr) / sizeof(char)); i++) {
    HC12.write(arr[i]);
    Serial.println(arr[i]);
  }
  int num = 0;
  HC12.write(num);
  Serial.println(0);
}

void sendStrings()
{
  String wrd = "<apple>";
  HC12.println(wrd);
  delay(10);
  HC12.end();
}

void sendCharArray()
{
  char words[]  = "hello";
  for (int i = 0; i<=5; i++)
  {
    char c = words[i];
    
  }
}

long functionOne() {
  randomSeed(analogRead(0));
  long randOne = random(0, 30);
  return randOne;
}

long functionTwo() {
  randomSeed(analogRead(0));
  long randTwo = random(40, 90);
  return randTwo;
}

long functionThree() {
  randomSeed(analogRead(0));
  long randThree = random(100, 180);
  return randThree;
}

long functionFour() {
  randomSeed(analogRead(0));
  long randFour = random(200, 330);
  return randFour;
}

long  functionFive() {
  randomSeed(analogRead(0));
  long randFive = random(400, 630);
  return randFive;
}
