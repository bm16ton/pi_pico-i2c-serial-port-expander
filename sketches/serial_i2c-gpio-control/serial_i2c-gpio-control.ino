uint16_t receivedPacket[10]; 
#include <Wire.h>


int int_array[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
String pin_str = "";

byte bytesSent = 0;
byte buffer[2];

uint16_t returninfo;

int led = LED_BUILTIN; // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int ii = 2;ii<=sizeof(int_array)/sizeof(int_array[0]);ii++){
    pinMode(ii,OUTPUT);
  }
  delay(30);
  Serial.println("Poop...");
  Wire.setSDA(0);
  Wire.setSCL(1);
//  Wire.begin(0x01); 
//  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  Wire.begin(0x77);      // join i2c bus with Slave ID
  Wire.onReceive(receiveDataPacket);
  Wire.onRequest(slavesRespond);  // register callback event
}

void loop() {
  while (Serial.available()){
    char in_char = Serial.read();
    if (int(in_char)!=-1){
      pin_str+=in_char;
    }
    if (in_char=='\n'){
      int pin_num = pin_str.toInt();
      int_array[pin_num] = !int_array[pin_num];
      digitalWrite(pin_num,int_array[pin_num]);
      Serial.print("Pin # ");
      Serial.print(pin_num);
      if (int_array[pin_num]==0){
        Serial.println(" OFF");
      } else {
        Serial.println(" ON");
      }
      pin_str = "";
    } 
  }
}

void receiveDataPacket(int howMany){
  // get bytes from i2c
  bytesSent = 0; // clear byte counter
  for(byte i=0; i < howMany; i++){
    receivedPacket[i] = Wire.read();
  } 
/*
  receivedPacket[0] = method
*/
}

void slavesRespond(){
  if(bytesSent == 0){
    switch(receivedPacket[0]){
    case 1:  // digitalWrite
      /*
      receivedPacket[1] = pin
      receivedPacket[2] = value
      */
 //     Serial.println("slaveresponse  digitalwrite");
      pinMode(receivedPacket[1], OUTPUT); // set pin mode
      digitalWrite(receivedPacket[1], receivedPacket[2]);
      returninfo = 1;
      break;
    case 2:  // digitalRead
      /*
      receivedPacket[1] = pin
      */
 //     Serial.println("slaveresponse  digitalread");
      pinMode(receivedPacket[1], INPUT); // set pin mode
      returninfo = digitalRead(receivedPacket[1]);
      break;
    case 3:  // digitalRead pullup
//      Serial.println("slaveresponse  digitalreadpullup");
      /*
      receivedPacket[1] = pin
      */
      pinMode(receivedPacket[1], INPUT_PULLUP); // set pin mode
      returninfo = digitalRead(receivedPacket[1]);
      break;
    case 4:  // analogWrite
      /*
      receivedPacket[1] = pin
      receivedPacket[2] = value
      */
 //     Serial.println("slaveresponse  analogwrite");
      pinMode(receivedPacket[1], OUTPUT); // set pin mode
      analogWrite(receivedPacket[1], receivedPacket[2]);
      returninfo = 1;
      break;
    case 5:   // analogRead
      /*
      receivedPacket[1] = pin
      */
      returninfo = analogRead(receivedPacket[1]);
      break;
        }
  } 
  if(bytesSent == 0){ //send first byte
    buffer[0] = returninfo >> 8;
    buffer[1] = returninfo & 0xff;
    Wire.write(buffer[0]);
    bytesSent++;
  } else if(bytesSent == 1){ // send second byte
    Wire.write(buffer[1]);
    bytesSent = 0; // clear byte counter
  }
}
