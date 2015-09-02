#include <SoftwareSerial.h>
#include <WIZ610.h>
#include <SPI.h>
#include <RFID.h>

#define BUF_LENGTH 100
#define READ 1
#define txWifi 3
#define rxWifi 2

#define SS_PIN 7
#define RST_PIN 9

char cmd;
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;
String tagid = " ";

SoftwareSerial wifiPort(rxWifi,txWifi);
WIZ610 modem(&wifiPort);
RFID rfid(SS_PIN, RST_PIN);

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

  modem.setWIRCfg(WIZ610_B_G, WIZ610_CLIENT, "Ninja", WIZ610_CH_AUTO);
  modem.setSecurity(WIZ610_WPA2_PSK, WIZ610_TKIP_AES, WIZ610_TKIP, WIZ610_ASCII, WIZ610_PASSPHRASE, "A2B4C6D8E0");

  Serial.print("IP: ");
  Serial.println(modem.getIP());
  Serial.println("Restarting...");
  modem.restart();
  delay(45000);
  modem.exitConfig();
  Serial.println("Terminal");

  SPI.begin();
  rfid.init();
}

void loop() {
  readTAG();
}

void readTAG(){
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      Serial.println(" ");
      Serial.println("Card found");
      serNum0 = rfid.serNum[0];
      serNum1 = rfid.serNum[1];
      serNum2 = rfid.serNum[2];
      serNum3 = rfid.serNum[3];
      serNum4 = rfid.serNum[4];

      Serial.println("Cardnumber:");

      tagid = String(rfid.serNum[0],HEX) + String(rfid.serNum[1],HEX) + String(rfid.serNum[2],HEX) + String(rfid.serNum[3],HEX) + String(rfid.serNum[4],HEX);
      Serial.println(tagid);
    }
  }

  rfid.halt();
}


