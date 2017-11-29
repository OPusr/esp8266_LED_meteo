/*
LEDmeteo 
https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples
https://github.com/stelgenhof/NTPClient
http://arduino.ru/Reference/Serial/Println
*/

//#include <Esp.h>
//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
//#define DEBUG_NTPClient
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <math.h>
//#include <TimeLib.h>

#include "flash_tools.h"
#include "wifi_tools.h"
#include "pass.h"

  //GPIO  16  5   4   0   2   14  12  13  15  3   1
  //      D0  D1  D2  D3  D4  D5  D6  D7  D8  D9  D10
  //      -   SCL SDA -   LED

#define LED_PIN D4 // GPIO2   //#define LED_PIN 2 // GPIO2

#define t_1m  80000
#define t_10m  800000
#define t_100m  8000000   //8332000 ?
#define t_500m  40000000   //41660000 ?
#define t_1s  80000000

#define t_n 5 //main program loops per second
#define t_  t_1s/t_n

short int time_cnt=0;
unsigned long uptime=0;
unsigned long uptime_old=0;

// NTP Servers:
static const char ntpServerName[] = "ru.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
const int timeZone = 3;     // Moscow

WiFiUDP ntpUDP;

//NTPClient timeClient(ntpUDP, ntpServerName, 3600*timeZone, 1000*60);
NTPClient timeClient(ntpUDP, ntpServerName1, 3600*timeZone, 1000*60);
//setTimeOffset(3600*timeZone); //in seconds
//setUpdateInterval(1000*60); //in milliseconds

bool ledState;
void blink() {
  digitalWrite(LED_PIN, ledState);
  ledState = !ledState;
}

bool loop_run = false;
unsigned long time_new=0;
unsigned long time_old=0;

void timer_f(void){
  timer0_write(ESP.getCycleCount() + t_);
  loop_run=true;
  time_old=time_new;
  //time_new=millis();
  time_new=micros();

  blink();

  //digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
  //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}


/*time_t getNtpTime()
{
  return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
}*/
//////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(10);  //10 ms
  
  flash_info ();
  
  pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, LOW);
  //digitalWrite(LED_PIN, HIGH);
  
  wifi_scan ();
  Serial.print("Connecting to "); // Connect to WiFi network
  #ifndef PASS_H
    const char* ssid = "***";
    const char* password = "***";
  #endif
  //Serial.println(ssid);
  Serial.println(ssid1);
  //WiFi.begin(ssid, password);
  WiFi.begin(ssid1, password1);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  wifi_info();

  timeClient.begin();

///*
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer_f);
  timer0_write(ESP.getCycleCount() + t_);
  interrupts();//*/

  /*  // Start the server
  WiFiServer server(80); //Create an instance of the server, specify the port to listen on as an argument
  server.begin();
  Serial.println("Server started");*/

  WiFiClient client;
  if(!client.connect("ya.ru", 80))
  {
    Serial.println("\r\n...... Internet Connection test failed...Retrying....\r\n");
  }
  else
  {
    Serial.println("internet is ok");
  }

  /*
  HTTPClient http;
  int httpCode;
  String payload;
  //String url = "http://"+Config.serverIP+"/objects/?object="+Config.name+"&op=set&p="+Config.property+"&v=";
  //String url = "http://ya.ru";
  String url = "http://ya.ru/favicon.ico";
  //String url = "http://sms.ru";
  http.begin(url);
  httpCode = http.GET();
  Serial.println(httpCode);
  payload = http.getString();
  Serial.println(payload);
  if(httpCode == HTTP_CODE_OK)
  {
    Serial.print("HTTP response code ");
    Serial.println(httpCode);
    String response = http.getString();
    Serial.println(response);
  }
  else
  {
    Serial.println("Error in HTTP request");
  }
    http.end();
  //*/

}
  
  

//////////////////////////////////////////////////////////////////
void loop() {
  if(loop_run==true){
    loop_run=false;
    //Serial.println(time_new-time_old);

    //Serial.println(time_new);
    //Serial.println(time_old);   
    //Serial.println(micros());
    //Serial.println(millis());
    //Serial.println(ESP.getCycleCount()); 

    //Serial.println("uptime");
    //Serial.println(uptime);
    if (time_cnt==t_n){
      time_cnt=0;
      uptime++;
      //Serial.println(uptime);
      if (uptime%5==0){
        Serial.println(uptime);
      }
    }
    else{
      time_cnt++;
    }

    if (uptime%5==0){
      Serial.println(uptime);
    }

  }

  timeClient.update();    //get time from NTP server
  //timeClient.forceUpdate();  
  //Serial.println(timeClient.getFormattedTime());
  //Serial.println(timeClient.getEpochTime());
  /*
  Serial.println(timeClient.getDay());
  Serial.println(timeClient.getHours());
  Serial.println(timeClient.getMinutes());
  Serial.println(timeClient.getSeconds());//*/

  //digitalWrite(LED_PIN, LOW);
  //analogWrite(LED_PIN, PWMRANGE-0);
  //delay(2500);
  //digitalWrite(LED_PIN, HIGH);
  //analogWrite(LED_PIN, PWMRANGE-23);
  //delay(2500);
}


//////////////////////////////////////////////////////////////////
/* 
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

 //http://server_ip/gpio/0 will set the GPIO2 low,
 //http://server_ip/gpio/1 will set the GPIO2 high
  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(2, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"high":"low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

  ////////////
    const char* host = "http://ya.ru";
    if (client.connect(host, 80)) {
    Serial.println("Sending...");
    client.print("GET /index.html");
    client.println(" HTTP/1.1\r\n");
    client.print( "Host: " );
    client.println(host);
    client.println( "Connection: close" );
    client.println();
    client.println();
    client.flush();
    client.stop();
    Serial.println("End sending!");
    delay(100);
  /////////

//*/
