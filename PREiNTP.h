#ifndef PREi_NTP_H
#define PREi_NTP_H

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define NTP_PACKET_SIZE 48
#define TIMEOUT_MS 200

class PREiNTP
{
  private:
    WiFiUDP _udp;
    byte _packet_buffer[NTP_PACKET_SIZE];
    IPAddress _ntp_server_ip;
    unsigned long _last_unix;
    unsigned long _last_millis;
    bool _unix_old;
    void sendNTPpacket();
    unsigned long getOfflineUnix();

  public:
    PREiNTP();
    unsigned long getUnix();
    unsigned long getLastMillis();
    bool isUnixOld();
};

#endif
