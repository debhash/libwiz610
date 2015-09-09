#include <SoftwareSerial.h>
#include <WIZ610.h>

#define   PULL_UPS_OFF     bitSet(MCUCR,PUD)
#define   txWifi          3
#define   rxWifi          2
#define   LIB_DOMAIN      "192.168.0.2"
#define   LIB_DOMAIN_PORT "8080"

SoftwareSerial wifiPort(rxWifi, txWifi);
WIZ610 modem(&wifiPort);

char buff[BUF_LENGTH];

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
  modem.serialClientCfg(WIZ610_TCP, LIB_DOMAIN, LIB_DOMAIN_PORT, "60");

  Serial.println("ready");
  Serial.print("exit config...");
  
  modem.exitConfig();

  Serial.println("done.");

  Serial.print("IP: ");
  Serial.println(modem.getIP());
}

void loop() {
  getToken();
  modem.receive(buff);
  
  Serial.print("token: ");
  Serial.println(buff);

  delay(5000);
}

void getToken() {
  modem.sendln("GET /getToken/h1h1h1 HTTP/1.1");
  modem.sendln("Host: 192.168.0.2");
  modem.sendln("Connection: close");
  modem.sendln();
}


