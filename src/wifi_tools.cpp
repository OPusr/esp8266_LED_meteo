#include <esp.h>
#include <ESP8266WiFi.h>

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