#include <WiFiServer.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFiAP.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiClient.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <WiFiClientSecureAxTLS.h>
#include <ESP8266WiFi.h>
#include <WiFiServerSecureAxTLS.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiServerSecure.h>
#include <time.h>

const char* ssid = "ShypishWIFI";
const char* password = "Shipish89075074";

int zone1 = D2;
int zone2 = D3;
int zone3 = D4;
int zone4 = D5;


WiFiServer server(80);
 
void setup() {
  Serial.begin(9600);
  delay(10);
 
 
  pinMode(zone1, OUTPUT);
  digitalWrite(zone1, HIGH);
  pinMode(zone2, OUTPUT);
  digitalWrite(zone2, HIGH);
  pinMode(zone3, OUTPUT);
  digitalWrite(zone3, HIGH);
  pinMode(zone4, OUTPUT);
  digitalWrite(zone4, HIGH);
  
  connectToWifi();

  // set time
  configTime(timezone * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  
 
}
 
void loop() {
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
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  
  if (request.indexOf("/zone1=ON") != -1) {
    digitalWrite(zone1, LOW);
  } else if (request.indexOf("/zone1=OFF") != -1){
    digitalWrite(zone1, HIGH);
  }
  
  if (request.indexOf("/zone2=ON") != -1) {
    digitalWrite(zone2, LOW);
  } else  if (request.indexOf("/zone2=OFF") != -1){
    digitalWrite(zone2, HIGH);
  }

  if (request.indexOf("/zone3=ON") != -1) {
    digitalWrite(zone3, LOW);
  } else  if (request.indexOf("/zone3=OFF") != -1){
    digitalWrite(zone3, HIGH);
  }
 
 
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE html>"
"<html>"
"<head>"
  "<meta charset='utf-8'>"
  "<title>Полив</title>"
"</head>"
"<style type='text/css'>"
 "body button {"
    "font-size: 4rem;"
  "}"
"</style>"
"<body>"
  "<div align='center'>"
    "<h4>Реле 1</h4>"
    "<a href='/zone1=ON'><button>вкл</button></a> <a href='/zone1=OFF'><button>викл</button></a>"
    "<h4>Реле 2</h4>"
    "<a href='/zone2=ON'><button>вкл</button></a> <a href='/zone2=OFF'><button>викл</button></a>"
    "<h4>Реле 3</h4>"
    "<a href='/zone3=ON'><button>вкл</button></a> <a href='/zone3=OFF'><button>викл</button></a>"
    "<h4>Реле 4</h4>"
    "<a href='/zone4=ON'><button>вкл</button></a> <a href='/zone4=OFF'><button>викл</button></a>"
  "</div>"
"</body>"
"</html>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("On");  
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}








/**
 * Conneting to wifi 
 * and begin the server
 **/
void connectToWifi() {
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}