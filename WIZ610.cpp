#include "WIZ610.h"
#include <SoftwareSerial.h>
#include <string.h>
#include <avr/pgmspace.h>

WIZ610::WIZ610(SoftwareSerial *modemPort) {
  pinMode(CFG_PIN, OUTPUT);
  pinMode(PWR, OUTPUT);
  pinMode(AUX_LED, OUTPUT);

  modem = modemPort;
  modem->begin(9600);
}

/**
Checks  serial baudrate value and changes to 9600 bps. This
baudrate is recommended to use with softserial library.
@param void
@return void
*/
void WIZ610::initBaudrate(void) {
  char buf[50];
  int i;
  char *ptr;

  modem->begin(38400);
  modem->print("<E");
  modem->flush();

  getsTimeout(buf, 4000);
  delay(2000);

  ptr = strstr(buf, "<1>");

  if(ptr){
    ledOn();
    while( serialCfg(WIZ610_9600, WIZ610_8BIT, WIZ610_NO_PARITY, WIZ610_NO_FLOW_CTRL, WIZ610_1STOPBIT) != WIZ610_SUCCESS );
    delay(500);
    pwrOff();

    delay(5000);
    pwrOn();

    delay(45000);
    ledOff();
  }

  modem->end();
  modem->begin(9600);
  modem->flush();
  modem->read();
}

/**
Serial to wireless / Serial parameters
@param baudrate Configure serial communication speed
            WIZ610_115200
            WIZ610_57600
            WIZ610_38400
            WIZ610_19200
            WIZ610_9600
            WIZ610_4800
            WIZ610_2400
            WIZ610_1200
@param databits Configure databits
            WIZ610_7BIT
            WIZ610_8BIT
@param parity Configure parity checking option
            WIZ610_NO_PARITY
            WIZ610_ODD
            WIZ610_EVEN
@param flowCtrl Configure flow control option
            WIZ610_NO_FLOW_CTRL
            WIZ610_XON/XOFF
            WIZ610_RTS/CTS
@param stopbits Configure stop bit option
            WIZ610_1STOPBIT
            WIZ610_2STOPBIT
@return status
*/
byte WIZ610::serialCfg(byte baudrate, byte databits, byte parity, byte flowCtrl, byte stopbits){
  char str[60];
  byte ret = WIZ610_SUCCESS;
  char tmp[2];

  if(ret == WIZ610_SUCCESS){
    // Baudrate
    tmp[0] = baudrate + 48;
    tmp[1] = 0;
    strcpy (str,"WB");
    strcat (str, tmp);
    // Databits
    tmp[0] = databits + 48;
    tmp[1] = 0;
    strcat (str, tmp);
    // Parity
    tmp[0] = parity + 48;
    tmp[1] = 0;
    strcat (str, tmp);
    // Flow Control
    tmp[0] = flowCtrl + 48;
    tmp[1] = 0;
    strcat (str, tmp);
    // Stop Bits
    tmp[0] = stopbits + 48;
    tmp[1] = 0;
    strcat (str, tmp);

    ret = setModem(str, 5000);
  }
}

/**
Get IP
@param void
@return incomming byte
*/
char * WIZ610::getIP(void){
  char buff[BUF_LENGTH];
  getModem("RI", 2000, buff);
  return buff;
}

/**
Restars WIZ610 Module
@param void
@return void
*/
void WIZ610::restart(void) {
  char str[60];
  strcpy (str,"WR");
  setModem(str, 10000);
}

/**
Get data from modem
@param command Command is required
@param timeout Max. waiting time for current command execution
@param check Checks the right command execution
@param buf Temporal buffer for store incomming data
@return status

*/
byte WIZ610::getModem(const char *command, uint16_t timeout, char *buf) {

  byte count = 0;
  char *found = 0;
  byte ret = WIZ610_SUCCESS;
  *buf = 0;

  // Command send
  modem->print(SRT_DEL);
  modem->print(command);
  modem->print(END_DEL);
  modem->flush();

  // Wait for response until timeout expires
  count = getsTimeout(buf, timeout);    // Timeout review

  if (count) {
    ret = WIZ610_SUCCESS;
  }
  else {
    ret = WIZ610_NO_RESPONSE;
  }
  return ret;
}

/**
Set data to modem
@param command Command is required
@param timeout Max. waiting time for current command execution
@param check Checks the right command execution
@param buf Temporal buffer for store incomming data
@return status
*/
byte WIZ610::setModem(const char *command, uint16_t timeout) {

  byte count = 0;
  byte ret = WIZ610_SUCCESS;
  char buf[15];

  // Command send
  modem->print(SRT_DEL);
  modem->print(command);
  modem->print(END_DEL);
  modem->flush();

  count = getsTimeout(buf, timeout);    // Timeout review
  buf[3] = 0;
  if (count) {
    switch(buf[1]){
      case 'S':
        ret = WIZ610_SUCCESS;
      break;
      case 'F':
        ret = WIZ610_FAIL;
      break;
      case '0':
        ret = WIZ610_INVALID_STX;
      break;
      case '1':
        ret = WIZ610_INVALID_CMD;
      break;
      case '2':
        ret = WIZ610_INVALID_PARAM;
      break;
      case '3':
        ret = WIZ610_INVALID_ETX;
      break;
      case '4':
        ret = WIZ610_DONT_WORK;
      case 'E':
        ret = WIZ610_ENTER_SER_CMD;
      break;
       default:
         Serial.println("Unknown Error");
       break;
    }

  }
  else {
      ret = WIZ610_NO_RESPONSE;
  }
  return ret;
}

/**
Wireless security setup for the clients connecting to Access Point
@param authMode Authentication Method
                WIZ610_OP_SH: Open or Shared Key
                WIZ610_OPEN: Client authentication is performed by RADIUS server
                WIZ610_802_1X: 802.1x
                WIZ610_SHARED: Shared Key
                WIZ610_WPA: WiFi Protected Access
                WIZ610_WPA_PSK: WPA Pre-Shared Key
                WIZ610_WPA2: WPA with AES Encryption
                WIZ610_WPA2_PSK: WPA with Advanced Encryption Standard (AES) for encryption keys together whith WPA PSK method
@param encrypt Configures authentication mode for security of wireless networks
                WIZ610_NONE: No encryption is used
                WIZ610_WEP: WEP encryption
                WIZ610_TKIP: Temporal key integrity Protocol
                WIZ610_AES: Advanced Encryption Standard
                WIZ610_TKIP_AES: TKIP with AES
@param keyLength Configure the length of WEP Key
                WIZ610_WEP64: 64 bit
                WIZ610_WEP128: 128 bit
@param keyFormatWEP Configure the format of WEP Key
                WIZ610_ASCII: ASCII (5 characters)
                WIZ610_HEX: Hex (10 characters)
@param keyFormatWPA Configure the format of WPA key
                WIZ610_PASSPHRASE
                WIZ610_HEX
@param keyValue Pointer to authentication key (8-63 bytes)

@return status
*/
byte WIZ610::setSecurity(byte authMode, byte encrypt, byte keyLength, byte keyFormatWEP, byte keyFormatWPA, char *keyValue){
  char str[70];
  byte ret = WIZ610_SUCCESS;
  char tmp[2];

  // Set Authentication Mode
  tmp[0] = authMode + 48;
  tmp[1] = 0;
  strcpy (str,"GU");
  strcat (str, tmp);
  strcat(str,"_");

  // Set Encryptation
  tmp[0] = encrypt + 48;
  tmp[1] = 0;
  strcat (str, tmp);
  strcat(str,"_");

  // Set Key Length
  tmp[0] = keyLength + 48;
  tmp[1] = 0;
  strcat (str, tmp);
  strcat(str,"_");

  // Set WEP Key Format
  tmp[0] = keyFormatWEP + 48;
  tmp[1] = 0;
  strcat (str, tmp);
  strcat(str,"_");

  // Set WPA-PSK Key Format
  tmp[0] = keyFormatWPA + 48;
  tmp[1] = 0;
  strcat(str, tmp);
  strcat(str,"_");

  // Set WPA-PSK KeyValue
  strcat(str, keyValue);

  ret = setModem(str, 30000);

  return ret;
}

/**
Wireless client access point configuration
@param wirband Select a communication protocol
                WIZ610_B_G: 802.11 b/g
                WIZ610_B: 802.11 b
                WIZ610_G: 802.11 g
                WIZ610_N: 802.11 n
                WIZ610_B_G_N: 802.11 b/g/n
@param opMode Select module's operation mode
                WIZ610_AP: Access Point
                WIZ610_GW: Gateway
                WIZ610_AP_WDS: Access Point + WDS
                WIZ610_CLIENT: Client
@param SSID Pointer to WiFi network name string (1-32 char)
@param channel WiFi channel frequency
                WIZ610_CH_AUTO
                WIZ610_CH_1
                WIZ610_CH_2
                WIZ610_CH_3
                WIZ610_CH_4
                WIZ610_CH_5
                WIZ610_CH_6
                WIZ610_CH_7
                WIZ610_CH_8
                WIZ610_CH_9
                WIZ610_CH_10
                WIZ610_CH_11
                WIZ610_CH_12
                WIZ610_CH_13
@return status
*/
byte WIZ610::setWIRCfg(byte wirBand, byte opMode, char *SSID, byte channel){
  char str[60];
  byte ret = WIZ610_SUCCESS;
  char tmp[2];


  // Set wireless Band
  if(ret == WIZ610_SUCCESS){
    tmp[0] = wirBand + 48;
    tmp[1] = 0;
    strcpy (str,"GB");
    strcat (str, tmp);
    ret = setModem(str, 20000);

  }
  // Set Operation Mode
  if(ret == WIZ610_SUCCESS){
    tmp[0] = opMode + 48;
    tmp[1] = 0;
    strcpy (str,"GO");
    strcat (str, tmp);
    ret = setModem(str, 45000);
  }
  // Set SSID
  if(ret == WIZ610_SUCCESS){
    strcpy (str,"GS");
    strcat (str, SSID);
    ret = setModem(str, 2000);
  }

  // Set Channel
  if(ret == WIZ610_SUCCESS){
    tmp[0] = channel + 48;
    tmp[1] = 0;
    strcpy (str,"GC");
    strcat (str, tmp);
    ret = setModem(str, 2000);
  }

  return ret;
}

/**
Send char through WiFi module
@param buf Pointer to output buffer string
@return void
*/
void WIZ610::send(char *buf) {
    modem->print(buf);
}

/**
Send char with CR through WiFi module
@param buf Pointer to output buffer string
@return void
*/
void WIZ610::sendln(char *buf) {
    modem->println(buf);
}

/**
Send unsigned int with CR through WiFi module
@param buf Pointer to output buffer string
@return void
*/
void WIZ610::sendln(unsigned int n){
    modem->println( (unsigned long) n);
}

/**
Module enters in configuration mode
@param void
@return void
*/
void WIZ610::enterConfig(void){
  digitalWrite(CFG_PIN, HIGH);
}

/**
Module leaves configuration mode
@param void
@return void
*/
void WIZ610::exitConfig(void){
  digitalWrite(CFG_PIN, LOW);
}

/**
Power on Module
@param void
@return void
*/
void WIZ610::pwrOn(void){
  digitalWrite(PWR, LOW);
}


/**
Power off Module
@param void
@return void
*/
void WIZ610::pwrOff(void){
  digitalWrite(PWR, HIGH);
}

/**
Turn On onboard LED
@param void
@return void
*/
void WIZ610::ledOn(void){
  digitalWrite(AUX_LED, LOW);
}

/**
Turn Off onboard LED
@param void
@return void
*/
void WIZ610::ledOff(void){
  digitalWrite(AUX_LED, HIGH);
}

/**
Timeout review
@param buf Temporal buffer for store incomming data
@param timeout Max. waiting time for current command execution
@return Received characters
*/
byte WIZ610::setsTimeout(char *buf, uint16_t timeout) {

  byte count = 0;
  long timeIsOut = 0;
  char c;
  byte ret = WIZ610_FAIL;
  *buf = 0;
  timeIsOut = millis() + timeout;

  do{
    while(modem->available()){
      count++;
      c = modem->read();
      *buf++ = c;
    }
    if(strstr(buf, response)){
        ret = WIZ610_SUCCESS;
    }
  }
  while(timeIsOut > millis() && count < (BUF_LENGTH - 1) && ret == WIZ610_SUCCESS);

  if(ret != WIZ610_SUCCESS){
    if(timeIsOut <= millis()){
      ret = WIZ610_TIMEOUT;
    }
    else if(count >= (BUF_LENGTH - 1)){
     ret = WIZ610_BUFFER_OVERFLOW ;
    }
  }
  if (count != 0) {
    *buf = 0;
    count++;
  }
  return ret;
}


byte WIZ610::getsTimeout(char *buf, uint16_t timeout) {
  byte count = 0;
  long timeIsOut = 0;
  char c;
  *buf = 0;
  timeIsOut = millis() + timeout;
  while (timeIsOut > millis() && count < (BUF_LENGTH - 1)) {
    if (modem->available()) {
      count++;
      c = modem->read();
      *buf++ = c;
      timeIsOut = millis() + timeout;
    }
  }
  if (count != 0) {
    *buf = 0;
    count++;
  }
  return count;
}