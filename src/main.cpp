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


// NTP Servers:
static const char ntpServerName[] = "ru.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
const int timeZone = 3;     // Moscow



WiFiUDP ntpUDP;
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
//NTPClient timeClient(ntpUDP, "ru.pool.ntp.org", 3600*3, 1000*60*10);
NTPClient timeClient(ntpUDP, "ru.pool.ntp.org", 3600*3, 1000*60);


bool loop_run = false;
unsigned long time_new=0;
unsigned long time_old=0;

void LEDblink(void){
  timer0_write(ESP.getCycleCount() + t_100m);
  loop_run=true;
  //digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);
  //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  time_old=time_new;
  //time_new=millis();
  time_new=micros();
}

bool ledState;
void blink() {
  digitalWrite(LED_PIN, ledState);
  ledState = !ledState;
}

/*time_t getNtpTime()
{
  return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
}*/
//////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(10);
  
  flash_info ();
  
  pinMode(LED_PIN, OUTPUT);
  //digitalWrite(LED_PIN, LOW);
  //digitalWrite(LED_PIN, HIGH);
  
  wifi_scan ();
  Serial.print("Connecting to "); // Connect to WiFi network
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //WiFi.begin(ssid1, password1);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  wifi_info();

  timeClient.begin();

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(LEDblink);
  //next=ESP.getCycleCount()+1000;
  timer0_write(ESP.getCycleCount() + t_100m);
  interrupts();

  /*  // Start the server
  WiFiServer server(80); //Create an instance of the server, specify the port to listen on as an argument
  server.begin();
  Serial.println("Server started");*/

  WiFiClient client;
  //if(!client.connect("google.com", 80))
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
  //String url = "https://hotspot.trimble.com/authentication.php?useGuest=1&username=guest&password=wigxindk";
  //String url = "http://ya.ru";
  String url = "http://ya.ru/favicon.ico";
  //String url = "http://sms.ru/sms/send?api_id=e821dbe4-c6e5-1f34-1183-d510d31b8a84&to=79104345491&text=hello+world";
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
  }

  timeClient.update();
  //timeClient.forceUpdate();  
  Serial.println(timeClient.getFormattedTime());
  Serial.println(timeClient.getEpochTime());
  /*Serial.println(timeClient.getDay());
  Serial.println(timeClient.getHours());
  Serial.println(timeClient.getMinutes());
  Serial.println(timeClient.getSeconds());*/

  /*digitalWrite(LED_PIN, LOW);
  delay(500);
  digitalWrite(LED_PIN, HIGH);
  delay(500);*/

  analogWrite(LED_PIN, PWMRANGE-0);
  delay(2500);
  analogWrite(LED_PIN, PWMRANGE-23);
  delay(2500);
}



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
