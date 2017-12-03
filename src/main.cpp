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
////#include <NTPClient.h>
#include <WiFiUdp.h>
#include <math.h>

#include <TimeLib.h>

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
//unsigned long uptime=4294967100;
unsigned long uptime=0;
unsigned long uptime_old=0;

bool AT_HOME=false;
bool AT_WORK=false;

//#define WORK

// NTP Servers:
//static const char ntpServerName[] = "time.nist.gov";
static const char* ntpServerName = "ru.pool.ntp.org";

const int timeZone = 3;     // Moscow

////WiFiUDP ntpUDP;
#ifdef WORK
NTPClient timeClient(ntpUDP, ntpServerName_w, 3600*timeZone, 1000*60);
#else
////NTPClient timeClient(ntpUDP, ntpServerName, 3600*timeZone, 1000*60);
#endif
//setTimeOffset(3600*timeZone); //in seconds
//setUpdateInterval(1000*60); //in milliseconds

bool ledState;
void blink() {
  //digitalWrite(LED_PIN, ledState);
  ledState = !ledState;

  //digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
  //digitalWrite(LED_PIN, !digitalRead(LED_PIN));

  if (ledState==0){
    digitalWrite(LED_PIN, LOW);
    analogWrite(LED_PIN, PWMRANGE-0);
  }
  else{
    digitalWrite(LED_PIN, HIGH);
    analogWrite(LED_PIN, PWMRANGE-23);
  }
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
}


//*
WiFiUDP Udp;
unsigned int localPort = 1337;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);
time_t prevDisplay = 0; // when the digital clock was displayed
//*/

//////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(10);  //10 ms
  
  flash_info ();
  
  pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, LOW);
  //digitalWrite(LED_PIN, HIGH);
  
  wifi_scan ();
  const char* ssid = "***";
  const char* password = "***";
/*
  #ifdef PASS_H
    bool AT_HOME=false;
    bool AT_WORK=false;
    if(AT_WORK){
      ssid=ssid_w;
      password=password_w;
      //ntpServerName=ntpServerName_w;
    }
    if (AT_HOME){
      ssid=ssid_h;
      password=password_h;
      //ntpServerName=ntpServerName_h;
    }
  #endif
  */
  Serial.print("Connecting to "); // Connect to WiFi network
  //Serial.println(ssid);
  //WiFi.begin(ssid, password);
#ifdef WORK
  Serial.println(ssid_w);
  WiFi.begin(ssid_w, password_w);
#else
  Serial.println(ssid_h);
  WiFi.begin(ssid_h, password_h);
#endif
  
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  wifi_info();

  ////timeClient.begin();
  ////timeClient.forceUpdate();
Udp.begin(localPort);
Serial.print("Local port: ");  //  "Локальный порт: "
Serial.println(Udp.localPort());
Serial.println("waiting for sync");  //  "ждем синхронизации"
  //*
  setSyncProvider(getNtpTime);
  setSyncInterval(45);

  //*/

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
      //Serial.println(uptime);
      if (uptime%5==0){
        //Serial.println(uptime);

        //unsigned long days = (((uptime)  / 86400L) + 4 ) % 7);
        unsigned long days = uptime  / 86400L;
        String daysStr = String(days) + " days,";
        
        unsigned long hours = (uptime % 86400L) / 3600;
        String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

        unsigned long minutes = (uptime % 3600) / 60;
        String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

        unsigned long seconds = uptime % 60;
        String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

        String uptimeStr = daysStr + " " + hoursStr + ":" + minuteStr + ":" + secondStr;
        Serial.print("Uptime: ");
        Serial.println(uptimeStr);
      }

      if (uptime%10==0){
        Serial.print("NTP time: ");
        //Serial.println(timeClient.getEpochTime());
        ////Serial.println(timeClient.getFormattedTime());
        now();
      }

      if (uptime%30==0){
        Serial.println("NTP time update");
        ////timeClient.forceUpdate();
      }

      uptime++;
    }
    else{
      time_cnt++;

      ////timeClient.update();    //get time from NTP server
      //timeClient.forceUpdate();  
      //Serial.println(timeClient.getFormattedTime());
      //Serial.println(timeClient.getEpochTime());
      /*
      Serial.println(timeClient.getDay());
      Serial.println(timeClient.getHours());
      Serial.println(timeClient.getMinutes());
      Serial.println(timeClient.getSeconds());//*/
    }

  }

  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      Serial.println("NTP time update 2");
      digitalClockDisplay();
      Serial.println(now());
    }
  }

}
//////////////////////////////////////////////////////////////////


void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/
/*
const int NTP_PACKET_SIZE_ = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE_]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
#ifdef WORK
  WiFi.hostByName(ntpServerName_w, ntpServerIP);
  Serial.print(ntpServerName_w);
#else
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
#endif
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE_) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE_);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}
*/


const int NTP_PACKET_SIZE_ = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE_]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
#ifdef WORK
  WiFi.hostByName(ntpServerName_w, ntpServerIP);
  Serial.print(ntpServerName_w);
#else
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
#endif
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);


  // Wait till data is there or timeout...
  byte timeout = 0;
  int cb = 0;
  do {
    delay ( 10 );
    cb = Udp.parsePacket();
    if (timeout > 100){
      Serial.println("No NTP Response :-(");
      return 0; // return 0 if unable to get the time
    }
    timeout++;
  } while (cb == 0);

Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE_);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;

/*
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE_) {
      
    }
  }
  */
}





// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE_);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  //Udp.beginPacket(address, 123); //NTP requests are to port 123
Serial.println(ntpServerName_w);
  Udp.beginPacket(ntpServerName_w, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE_);
  Udp.endPacket();
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
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
