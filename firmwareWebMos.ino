#include <NTPClient.h>
#include <WiFiUdp.h>

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

#define SSID            "ShypishWIFI"
#define SSID_PASS       "Shipish89075074"

#define TZ              1       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

time_t now;

/*
tm_year
tm_mon
tm_mday
tm_hour
tm_min
tm_sec
*/
struct tm * timeinfo;

Zone zone1(D2);
Zone zone2(D3);
Zone zone3(D4);
Zone zone4(D5);

// int zone1 = D2;
// int zone2 = D3;
// int zone3 = D4;
// int zone4 = D5;

WiFiServer server(80);
WiFiClient client;
 
void setup() {
  Serial.begin(9600);
  delay(10);
  
  zone1.setPin();
  zone2.setPin();
  zone3.setPin();
  zone4.setPin();
  
  connectToWifi();
  // waiting for connecting
  delay(1500);

  // Synchronize time useing SNTP. This is necessary to verify that
  // the TLS certificates offered by the server are currently valid.
  configTime(TZ_SEC, DST_SEC, "pool.ntp.org");  
  setTime();

  // Set client connected
  client = server.available();
}



void loop() {
 
  // When the client sends some data
  if (client.available()){
    Serial.println("new client");

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();
    handleRequest(request);
  }







 
 
  
  // if (request.indexOf("/zone1=ON") != -1) {
  //   digitalWrite(zone1, LOW);
  // } else if (request.indexOf("/zone1=OFF") != -1){
  //   digitalWrite(zone1, HIGH);
  // }
  
  // if (request.indexOf("/zone2=ON") != -1) {
  //   digitalWrite(zone2, LOW);
  // } else  if (request.indexOf("/zone2=OFF") != -1){
  //   digitalWrite(zone2, HIGH);
  // }

  // if (request.indexOf("/zone3=ON") != -1) {
  //   digitalWrite(zone3, LOW);
  // } else  if (request.indexOf("/zone3=OFF") != -1){
  //   digitalWrite(zone3, HIGH);
  // }
 
 
 
  // // Return the response
  // client.println("HTTP/1.1 200 OK");
  // client.println("Content-Type: text/html; charset=utf-8");
  // client.println(""); //  do not forget this one

 
  // delay(1);
  // Serial.println("Client disconnected");
  // Serial.println("");
 
  delay(10);
}








/**
 * Conneting to wifi 
 * and begin the server
 **/
void connectToWifi() {
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
 
  WiFi.begin(SSID, SSID_PASS); 
 
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


/**
 * set current time and convert to 
 * the usual state
 *
 **/
void setTime() {
  now = time(nullptr); // get current time as UNIX TIMESTAMP
  timeinfo = localtime(&now); // get local time in struct
  timeinfo->tm_year = timeinfo->tm_year+1900;
  timeinfo->tm_mon = timeinfo->tm_mon+1;
}


void handleRequest(String request) {
  Serial.println(request);

  // Return the response Header
  client.println("HTTP/1.1 200 OK");

  // if user requested date
  if (request.indexOf("/getData") != -1) {
    client.println("Content-Type: application/json");
    client.println(""); //  do not forget this one
    //@todo повернути черги поливу, значення і ін. в вигляді JSON
    return;
  }

  client.println("Content-Type: text/html; charset=utf-8");
  client.println(""); //  do not forget this one

  // if user sent a management command
  if (request.indexOf("/zone1") != -1) {
    zone1.control(request);
  } else if (request.indexOf("/zone2") != -1) {
    zone2.control(request);
  } else  if (request.indexOf("/zone3") != -1) {
    zone3.control(request);
  } else  if (request.indexOf("/zone4") != -1){
    zone4.control(request);
  } else {
    // send to user main page
    client.println(printPage());
  }


}




/**
 * Class that represent one zone
 * of watering zone
 *
 **/ 
class Zone {
  private:
    int pinRelay; // pin connect to relay


  public:
    unsigned long planingTimeFrom = 0; // TIMESTAMP when watering should turn on
    unsigned long realTimeFrom = 0; // TIMESTAMP real time, when watering was turn on
    unsigned long timeDuration = 0; // duration of watering (timeFrom + timeDuration) = Time of end watering
    boolean isWatering = FALSE; // if watering turn on at the moment

    Zone(int pin) {
      pinRelay=pin;
    };

    void setPin() {
      pinMode(pinRelay, OUTPUT);
      digitalWrite(pinRelay, HIGH);
    };
    void handleCommand(String request) {

    }
    // check if queue has jobs that shoud be run
    boolean checkQueue() {

    };
  
};



/**
 * Class to work with pump
 * it can turn on/off (emergency)
 **/
class Pump {
  private:
    int pinRelay; // pin connect to relay
    int pinAmperemeter = A0; // pin connect to Amperemeter

  public:
    unsigned long maxWorkingTime = 0; // TIMESTAMP max time for which pump should turn on in 1 cycle
    unsigned long TimeStart = 0; // TIMESTAMP real time, when pupm was turn on
    //unsigned long timeWorks = 0; // duration of watering (timeFrom + timeDuration) = Time of end watering
    boolean isWorks = FALSE; // if pomp turn on at the moment
    Pump(int pin) {
      pinRelay=pin;
    };
  
};







String printPage() {
  return "<!DOCTYPE html>"
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
"</html>";
}
