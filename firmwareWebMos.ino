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
    boolean isWatering = false; // if watering turn on at the moment

    Zone(int pin) {
      pinRelay=pin;
    };

    void setPin() {
      pinMode(pinRelay, OUTPUT);
      digitalWrite(pinRelay, HIGH);
    };
    void handleCommand(String request) {
      Serial.println(request);
      Serial.println("COMMAND HANDLING");

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
    boolean isWorks = false; // if pomp turn on at the moment
    Pump(int pin) {
      pinRelay=pin;
    };
  
};


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
  delay(1500);
  setTime();
  delay(1500);
  // Set client connected
 // client = server.available();
}



void loop() {
  client = server.available();
 
  // When the client sends some data
  if (client.available()){
    Serial.println("new client");

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    handleRequest(request);
    client.flush();
    
  }
//Serial.println(client.available());
//Serial.println(server.available());






 
 
  
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
  now = time(nullptr); // get current time as UNIX TIMESTAMP
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
    zone1.handleCommand(request);
  } else if (request.indexOf("/zone2") != -1) {
    zone2.handleCommand(request);
  } else  if (request.indexOf("/zone3") != -1) {
    zone3.handleCommand(request);
  } else  if (request.indexOf("/zone4") != -1){
    zone4.handleCommand(request);
  } else {
    
    // send to user main page
    client.print(printPage());
  }
//delay(400);

}










String printPage() {
  return "<!DOCTYPE html><html lang='uk'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'><title>Полив</title><link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css' integrity='sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T' crossorigin='anonymous'><style>body button{font-size:4rem;background-color:white}table{width:100%}table td{min-width:25px!important;min-height:25px!important;width:30px;height:30px;color:#f0f0f0}table td.zone{border:2px solid white;color:black}table td.zone1{background:linear-gradient(to top, #9b9b9b, #808080)}table td.zone2{background:linear-gradient(to left, #3cb371, #6ac98f, #3cb371)}table td.zone3{background:linear-gradient(to top, #9b9b9b, #808080)}table td.zone4{background:linear-gradient(to left, #3cb371, #6ac98f, #3cb371)}</style></head><body><div class='container p-0'><div class='row'><div class='col-md-6 offset-md-3 col-12 text-center'><table><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td colspan='4' rowspan='2' class='zone zone4' data-toggle='modal' data-target='#modalZone4'>4</td></tr><tr><td colspan='4' rowspan='3' class='zone zone1' data-toggle='modal' data-target='#modalZone1'>1</td><td colspan='2' rowspan='6' class='zone zone3' data-toggle='modal' data-target='#modalZone3'>3</td><td>.</td></tr><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr><tr><td>.</td><td colspan='3' rowspan='3' class='zone zone2' data-toggle='modal' data-target='#modalZone2'>2</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr><tr><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td><td>.</td></tr></table></div></div></div><hr><div class='row'><div class='col-md-6 offset-md-3 col-12 text-center'>Заплановані поливи</div></div><div class='row'><div class='col-md-6 offset-md-3 col-12 text-center'>......</div></div></div><div class='modal fade' id='modalZone1' tabindex='-1' role='dialog' aria-labelledby='modalZone1Title' aria-hidden='true'><div class='modal-dialog modal-dialog-centered' role='document'><div class='modal-content'><div class='modal-header'><h5 class='modal-title' id='exampleModalLongTitle'>Зона 1 Додати нове завдання</h5> <button type='button' class='close' data-dismiss='modal' aria-label='Close'> <span aria-hidden='true'>&times;</span> </button></div><div class='modal-body'><div class='row'><div class='col-3'>Початок:</div><div class='col-6'><input type='datetime-local' name='wateringStart'></div><div class='col-3'><label>Негайно <input type='checkbox' name='startNow'> </label></div></div><div class='row mt-3'><div class='col-3'>Тривалість:</div><div class='col-6'><input type='time' name='wateringStart'></div></div><div class='row mt-3'><div class='col-3'>Повторювати:</div><div class='col-4'><label><input type='radio' name='wateringRepead'> Одноразово</label></div><div class='col-3'><label><input type='radio' name='wateringRepead'> Щоденно</label></div></div><div class='modal-footer'> <button type='button' class='btn btn-secondary' data-dismiss='modal'>Закрити</button> <button type='button' class='btn btn-primary'>Додати</button></div></div></div></div><div class='modal fade' id='modalZone2' tabindex='-1' role='dialog' aria-labelledby='modalZone2Title' aria-hidden='true'><div class='modal-dialog modal-dialog-centered' role='document'><div class='modal-content'><div class='modal-header'><h5 class='modal-title' id='exampleModalLongTitle'>Зона 2</h5> <button type='button' class='close' data-dismiss='modal' aria-label='Close'> <span aria-hidden='true'>&times;</span> </button></div><div class='modal-body'> ...</div><div class='modal-footer'> <button type='button' class='btn btn-secondary' data-dismiss='modal'>Закрити</button> <button type='button' class='btn btn-primary'>Додати</button></div></div></div></div></body> <script src='https://code.jquery.com/jquery-3.3.1.slim.min.js' integrity='sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo' crossorigin='anonymous'></script> <script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js' integrity='sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1' crossorigin='anonymous'></script> <script src='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js' integrity='sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM' crossorigin='anonymous'></script> <script></script> </html>";
  
  
  
  }