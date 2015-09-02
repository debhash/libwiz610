#include "WIZ610.h"
#include <SoftwareSerial.h>
#include <string.h>
#include <avr/pgmspace.h>

WIZ610::WIZ610(SoftwareSerial *modem) {
  pinMode(CFG_PIN,OUTPUT);
  pinMode(PWR,OUTPUT);
  pinMode(AUX_LED, OUTPUT);

  modem->begin(9600);
}

/**
Get IP
@param alias Pointer to destination string
@return void
*/
char * WIZ610::getIP(){
  char buff[BUF_LENGTH];
  getModem("RI", 4000, buff);
  return buff + 1;
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
Module enters in configuration mode
@param void
@return void
*/
byte WIZ610::enterConfig(void){
  digitalWrite(CFG_PIN, HIGH);
  delay(2000);
  state |= STATE_CONFIG;

  return state;
}

/**
Module leaves configuration mode
@param void
@return void
*/
byte WIZ610::exitConfig(void){
  digitalWrite(CFG_PIN, LOW);
  delay(2000);
  state &= ~STATE_CONFIG;

  return state;
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