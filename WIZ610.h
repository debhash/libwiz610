#ifndef WIZ610_H
#define WIZ610_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#define BUF_LENGTH 100        // Rx Buffer length
#define CFG_PIN 12            // Configuration pin
#define AUX_LED 7             // Auxiliary Led
#define PWR 4                 // Power Pin

#define SRT_DEL '<'          // Start
#define END_DEL '>'          // End

#define STATE_OFF 0
#define STATE_ON 1
#define STATE_INITIALIZED 2
#define STATE_CONFIG 4
#define STATE_POSFIX 8

// Response codes
#define WIZ610_SUCCESS 0
#define WIZ610_NO_RESPONSE 254
#define WIZ610_FAIL 255
#define WIZ610_INVALID_STX 100
#define WIZ610_INVALID_CMD 101
#define WIZ610_INVALID_PARAM 102
#define WIZ610_INVALID_ETX 103
#define WIZ610_DONT_WORK 104
#define WIZ610_NO_MORE 105
#define WIZ610_TIMEOUT 106
#define WIZ610_BUFFER_OVERFLOW 107
#define WIZ610_ENTER_SER_CMD 1

// Configurations
#define WIZ610_B_G 0
#define WIZ610_B 2
#define WIZ610_G 3
#define WIZ610_N 6
#define WIZ610_B_G_N 9

#define WIZ610_AP 0
#define WIZ610_GW 1
#define WIZ610_AP_WDS 2
#define WIZ610_CLIENT 3

#define WIZ610_CH_AUTO 0
#define WIZ610_CH_1 1
#define WIZ610_CH_2 2
#define WIZ610_CH_3 3
#define WIZ610_CH_4 4
#define WIZ610_CH_5 5
#define WIZ610_CH_6 6
#define WIZ610_CH_7 7
#define WIZ610_CH_8 8
#define WIZ610_CH_9 9
#define WIZ610_CH_10 10
#define WIZ610_CH_11 11
#define WIZ610_CH_12 12
#define WIZ610_CH_13 13

#define WIZ610_OP_SH 0
#define WIZ610_OPEN 1
#define WIZ610_802_1X 2
#define WIZ610_SHARED 3
#define WIZ610_WPA 4
#define WIZ610_WPA_PSK 5
#define WIZ610_WPA2 6
#define WIZ610_WPA2_PSK 7

#define WIZ610_NONE 0
#define WIZ610_WEP 1
#define WIZ610_TKIP 2
#define WIZ610_AES 3
#define WIZ610_TKIP_AES 4

#define WIZ610_WEP64 1
#define WIZ610_WEP128 2

#define WIZ610_ASCII 0
#define WIZ610_HEX 1
#define WIZ610_PASSPHRASE 0

#define WIZ610_TCP 0
#define WIZ610_UDP 1

#define WIZ610_115200 1
#define WIZ610_57600 2
#define WIZ610_38400 3
#define WIZ610_19200 4
#define WIZ610_9600 5
#define WIZ610_4800 6
#define WIZ610_2400 7
#define WIZ610_1200 8

#define WIZ610_7BIT 7
#define WIZ610_8BIT 9

#define WIZ610_NO_PARITY 0
#define WIZ610_ODD 1
#define WIZ610_EVEN 2

#define WIZ610_NO_FLOW_CTRL 0
#define WIZ610_XON/XOFF 1
#define WIZ610_RTS/CTS 2

#define WIZ610_1STOPBIT 1
#define WIZ610_2STOPBIT 2


class WIZ610 {
  public:
    WIZ610(SoftwareSerial *modemPort);
    char * getIP();

    void send(char *buf);
    void sendln(char *buf = "");
    void sendln(unsigned int);
    char *receive(char *buf);
    byte enterConfig(void);
    byte exitConfig(void);
    void initBaudrate(void);
    void pwrOff(void);
    void pwrOn(void);
    void ledOn(void);
    void ledOff(void);
    void restart(void);

  private:
    char state;
    char response[];
    byte requestModem(const char *command, uint16_t timeout, boolean check, char *buf);
    byte getModem(const char *command, uint16_t timeout, char *buf);
    byte setModem(const char *command, uint16_t timeout);
    byte getsTimeout(char *buf, uint16_t timeout);
    byte setsTimeout(char *buf, uint16_t timeout);
    SoftwareSerial *modem;
};

#endif