/* 1byte HC12 Receiver example.
  /* Tutorial link: http://electronoobs.com/eng_arduino_tut96.php
   Code: http://electronoobs.com/eng_arduino_tut96_code1.php
   Scheamtic: http://electronoobs.com/eng_arduino_tut96_sch1.php
   Youtube Channel: http://www.youtube/c/electronoobs

  Module // Arduino UNO/NANO
    GND    ->   GND
    Vcc    ->   3.3VU
    Tx     ->   D10
    Rx     ->   D11
*/
#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
int greenLED = 6;
int yellowLED = 7;

int ndx = 0;

const byte numChars = 32;
char receivedChars[numChars];

boolean newData = false;

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(9, OUTPUT);
}
void loop() {
  for (int i = 1; i < 3; i++) {
    HC12.write(i);
    delay(1500);
    while (HC12.available() < 0){}
    while (HC12.available() > 0) {
      Serial.write(HC12.read());
    }
  }
}

void ledButtons() {
  if (HC12.available() > 0) {      // If HC-12 has data
    int val = HC12.read();
    if (val != -1) Serial.println(HC12.read());    // Send the data to Serial monitor
    if (val == 1) {
      digitalWrite(greenLED, HIGH);
      digitalWrite(yellowLED, LOW);
    }
    else if (val == 2) {
      digitalWrite(yellowLED, HIGH);
      digitalWrite(greenLED, LOW);
    }
    else if (val == 3) {
      digitalWrite(yellowLED, HIGH);
      digitalWrite(greenLED, HIGH);
      delay(100);
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, LOW);
      delay(100);
    }
    else {
      digitalWrite(yellowLED, LOW);
      digitalWrite(greenLED, LOW);
    }
  }
}

void receiveNumArray()
{
  int receivedChars[] = {};
  while (HC12.available() > 0) {
    int val = HC12.read();
    receivedChars[ndx] = val;
    Serial.println(receivedChars[ndx]);
    ndx++;
    delay(100);
  }
}

/***********************************RECEIVE WITH START AND END MARKERS****************************************/

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  // if (Serial.available() > 0) {
  while (HC12.available() > 0 && newData == false) {
    rc = HC12.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    newData = false;
  }
}
