#include <SoftwareSerial.h>
#include "WIZ610.h"

#define txWifi 3
#define rxWifi 2

SoftwareSerial wifiPort(rxWifi,txWifi);
WIZ610 modem(&wifiPort);

void setup() {
  modem.ledOff();
  modem.pwrOn();
  delay(10000);
  Serial.begin(9600);
  Serial.println("---------------------");
  Serial.println("Init");
  delay(40000);
  pinMode(txWifi,OUTPUT);
  pinMode(rxWifi,INPUT);
  bitSet(MCUCR,PUD);
  Serial.println("---------------------");
  Serial.println("ready");

  modem.enterConfig();

  Serial.print("IP: ");
  Serial.println(modem.getIP());

  modem.restart();
  delay(45000);
  modem.exitConfig();
}

void loop() {
  
}

