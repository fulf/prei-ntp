#include "PREiNTP.h"

PREiNTP::PREiNTP() {
  _udp.begin(2390);
  _last_unix = 0;
  _last_millis = 0;
  _unix_old = true;
};

unsigned long PREiNTP::getUnix() {
  _unix_old = true;

  if(WiFi.status() != WL_CONNECTED) {
      return PREiNTP::getOfflineUnix();
  }

  if(WiFi.hostByName("pool.ntp.org", _ntp_server_ip) != 1) {
    return -1;
  }

  PREiNTP::sendNTPpacket();

  unsigned long startWait = millis();
  while(!_udp.parsePacket()) {
    if((unsigned long)(millis() - startWait) >= TIMEOUT_MS) {
      return PREiNTP::getOfflineUnix();
    }
    yield();
  }

  _udp.read(_packet_buffer, NTP_PACKET_SIZE);
  unsigned long seconds_since_1990 =
    word(_packet_buffer[40], _packet_buffer[41]) << 16 |
    word(_packet_buffer[42], _packet_buffer[43]);
  _last_unix = seconds_since_1990 - 2208988800UL;
  _last_millis = millis();
  _unix_old = false;

  return _last_unix;
}

unsigned long PREiNTP::getLastMillis() {
  return _last_millis;
}

bool PREiNTP::isUnixOld() {
  return _unix_old;
}

void PREiNTP::sendNTPpacket() {
  memset(_packet_buffer, 0, NTP_PACKET_SIZE);
  _packet_buffer[0] = 0b11100011;   // LI, Version, Mode
  _packet_buffer[1] = 0;            // Stratum, or type of clock
  _packet_buffer[2] = 6;            // Polling Interval
  _packet_buffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  _packet_buffer[12]  = 49;
  _packet_buffer[13]  = 0x4E;
  _packet_buffer[14]  = 49;
  _packet_buffer[15]  = 52;

  _udp.beginPacket(_ntp_server_ip, 123); //NTP requests are to port 123
  _udp.write(_packet_buffer, NTP_PACKET_SIZE);
  _udp.endPacket();
}

unsigned long PREiNTP::getOfflineUnix() {
  if(_last_unix == 0) {
    return 0;
  } else {
    return _last_unix + (unsigned long)(millis() - _last_millis)/1000;
  }
}
