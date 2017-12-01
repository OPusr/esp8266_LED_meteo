#include <esp.h>
#include <ESP8266WiFi.h>
//#include "pass.h"

extern bool AT_HOME;
extern bool AT_WORK;
extern char* ssid_h;
extern char* ssid_w;
extern char* ntpServerName_h;
extern char* ntpServerName_w;

void wifi_scan (void){
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    for (int i = 0; i < n; ++i)
    {
      //int sec = WiFi.encryptionType(i);
      //if((sec == ENC_TYPE_NONE ) && (WiFi.RSSI(i) > -95))
      //char name = WiFi.SSID(i);
      Serial.println(WiFi.SSID(i));
      if (WiFi.SSID(i)==ssid_h){
        AT_HOME=true;
        Serial.println("at home");
      }
      if (WiFi.SSID(i)==ssid_w){
        AT_WORK=true;
        Serial.println("at work");
      }
      //WiFi.persistent(false);
      //WiFi.mode(WIFI_STA);
      //WiFi.disconnect();
	  }
  } 
}

void wifi_info (void){
  Serial.println(WiFi.localIP()); //IP
  Serial.println(WiFi.macAddress()); //MAC-адрес
  Serial.println(WiFi.subnetMask()); //маску подсети
  Serial.println(WiFi.gatewayIP()); //IP- шлюза
  Serial.println(WiFi.dnsIP()); //
  Serial.println(WiFi.SSID());// имя сети
  Serial.println(WiFi.RSSI()); //уровень сигнала
}