#include <SoftwareSerial.h>

/* Define receive and transmit pins */
#define rxPin 2
#define txPin 3
SoftwareSerial esp(rxPin , txPin);

/* Sends aggregated data every (DELAY * COUNTER) miliseconds */
const int DELAY = 1000;
const int COUNTER = 10;

/* Analog read pins */
const int lm35_pin = A0;	
const int LDR_pin = A1;

void setup() {
  Serial.begin(115200);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);  

  esp.begin(9600);
}

int counter = 0;
float totalTemp = 0;
float totalLDR = 0;

void loop() {
  int temp_adc_val;
  float tempVal;
  float LDRVal;
  float aggrTemp;
  float aggrLDR;


  LDRVal = analogRead(LDR_pin); /* Read LDR */
  temp_adc_val = analogRead(lm35_pin);	/* Read Temperature */
  tempVal = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  tempVal = (tempVal/10);	/* LM35 gives output of 10mv/°C */

  if (counter >= COUNTER) {
    aggrTemp = totalTemp / float(COUNTER);
    aggrLDR = totalLDR / float(COUNTER);
    String tempString = String(aggrTemp); 
    String LDRString = String(aggrLDR);
    String msg = String(tempString + "," + LDRString); 
    Serial.println("Sent message : " + msg); 
    esp.println(msg); 
    counter = 0;
    totalTemp = 0;
    totalLDR = 0;
  }
  else if (counter < COUNTER) {
    totalTemp += tempVal;
    totalLDR += LDRVal;
    
    Serial.print("Read Values: Temperature = ");
    Serial.print(totalTemp);
    Serial.print(" Degree Celsius \n");
    Serial.print("Read Values: LDR = ");
    Serial.print(totalLDR);
    Serial.print("\n");
    
    counter += 1;
  }
  delay(DELAY);
}