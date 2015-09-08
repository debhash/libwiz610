#include <SoftwareSerial.h>
#include <WIZ610.h>

#define  PULL_UPS_OFF   bitSet(MCUCR,PUD)

#define  txWifi         3
#define  rxWifi         2

SoftwareSerial wifiPort(rxWifi, txWifi);
WIZ610 modem(&wifiPort);

void setup() {
  modem.pwrOn();

  delay(10000);

  Serial.begin(9600);
  
  Serial.println("Init");
  Serial.print("waiting for config...");

  delay(40000);

  pinMode(txWifi, OUTPUT);
  pinMode(rxWifi, INPUT);
  PULL_UPS_OFF;

  modem.enterConfig();
  modem.initBaudrate();

  Serial.println("ready");
  Serial.print("connecting to router...");

  modem.setWIRCfg(WIZ610_B_G, WIZ610_CLIENT, "Ninja", WIZ610_CH_AUTO);
  modem.setSecurity(WIZ610_WPA2_PSK, WIZ610_TKIP_AES, WIZ610_TKIP, WIZ610_ASCII, WIZ610_PASSPHRASE, "A2B4C6D8E0");

  Serial.println("ready");
  Serial.print("exit config...");
  
  modem.exitConfig();

  Serial.println("done.");

  Serial.print("IP: ");
  Serial.println(modem.getIP());
}

void loop() {

}


