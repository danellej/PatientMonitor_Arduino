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

  int s = analogRead(0);
  delay(10);
  randomSeed(s);
  
  //  pinMode(button1, INPUT_PULLUP);
  //  pinMode(button2, INPUT_PULLUP);
  //  pinMode(button3, INPUT_PULLUP);
}
void loop() {
  // sendStrings();
  sendNumArray();
  delay(2000);
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

void sendNumArray()
{
  char one[10];
  char two[10];
  char thr[10];
  char fou[10];
  char fve[10];

  char e[] = "<";
  char f[] = ">";
  
  char sendNums[50];
  itoa(functionOne(), one, 10);
  itoa(functionTwo(),two, 10);
  itoa(functionThree(),thr, 10);
  itoa(functionFour(),fou, 10);
  itoa(functionFive(),fve, 10);
  
//  strcpy(sendNums, one);
  strcpy(sendNums, e);
  strcat(sendNums, one);
  strcat(sendNums, ",");
  strcat(sendNums, two);
  strcat(sendNums, ",");
  strcat(sendNums, thr);
  strcat(sendNums, ",");
  strcat(sendNums, fou);
  strcat(sendNums, ",");
  strcat(sendNums, fve);
  strcat(sendNums, f);
  strcat(sendNums,"\r\n");

//  HC12.print("#");
  HC12.print(sendNums);
  Serial.println(sendNums);
//  delay(100);
}

void sendStrings()
{
  String wrd = "<apple>";
  HC12.println(wrd);
  delay(10);
  HC12.end();
}

int functionOne() {
  int randOne = random(0, 30);
  randOne = 12;
  return randOne;
}

int functionTwo() {
  int randTwo = random(40, 90);
  randTwo = 13;
  return randTwo;
}

int functionThree() {
  int randThree = random(100, 180);
  randThree = 14;
  return randThree;
}

int functionFour() {
  int randFour = random(200, 330);
  randFour = 15;
  return randFour;
}

int  functionFive() {
  int randFive = random(400, 630);
  randFive = 16;
  return randFive;
}
