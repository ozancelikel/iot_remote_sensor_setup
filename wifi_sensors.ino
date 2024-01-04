#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>

const int RX_pin = D3; // for ESP8266 use 13  D7 on NodeMCU/wemos-d1-esp8266
const int TX_pin = D2; // for ESP8266 use 12  D6  on NodeMCU/wemos-d1-esp8266

// Serial Settings
SoftwareSerial Arduino(RX_pin, TX_pin);

// WiFi Settings
const char *ssid = "SSID";
const char *password = "PASSWORD";

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "SERVER_NAME";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 10000;

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendPOSTRequest(String temp, String ldr){
  if ((WiFi.status() == WL_CONNECTED)) {
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, serverName)) {  // HTTPS
      Serial.print("[HTTPS] POST...\n");
      // start connection and send HTTP header
      https.addHeader("Content-Type", "application/json");

      int httpResponseCode = https.POST((String)"{\"tempData\":\""+ temp +"\",\"ldrData\":\"" + ldr +"\"}");
      Serial.println("Message sent: " + (String)("{\"tempData\":\""+ temp +"\",\"ldrData\":\"" + ldr +"\"}"));
      // httpCode will be negative on error
      if (httpResponseCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpResponseCode);
        if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpResponseCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
}

void setup(){
	Serial.begin(115200);
	Arduino.begin(9600);

  pinMode(RX_pin, INPUT);
  pinMode(TX_pin, OUTPUT);  

  connectWiFi();
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop(){
	String inputStr;

  if(Arduino.available() > 0) {
    inputStr = Arduino.readStringUntil('\n');
    inputStr.trim();
    Serial.print("ESP: Message received -> ");
    Serial.print(inputStr);
    Serial.print("\n");

    int delimiter;
    delimiter = inputStr.indexOf(",");
    String tempString = inputStr.substring(0, delimiter + 1);
    String ldrString = inputStr.substring(delimiter + 1);

    sendPOSTRequest(tempString, ldrString);
  }
	delay(100);
}