/*
  WebSerial Demo
  ------
  This example code works for both ESP8266 & ESP32 Microcontrollers
  WebSerial is accessible at your ESP's <IPAddress>/webserial URL.

  Author: Ayush Sharma
  Checkout WebSerial Pro: https://webserial.pro
  
  tinkering4fun
  Modified to allow specifiying the HTML page.
  And to illustrate basic functionality of websocket I/O
*/
#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

// This contains an example for custom HTML / Javascript,
// may be replaced by your own code
#include <webserial_custompage.h>

AsyncWebServer server(80);

const char* ssid = ""; // Your WiFi SSID
const char* password = ""; // Your WiFi Password


/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    // WebSerial is accessible at "<IP Address>/webserial" in browser
    
    // WebSerial.begin(&server);
    // You may specify your own HTML with begin() ...
    WebSerial.begin(&server, 						// the server
					"/webserial", 					// URL
					WEBSERIAL_SIMPLE_HTML, 			// HTML char stream
					sizeof(WEBSERIAL_SIMPLE_HTML), 	// size of dto
					false							// unzipped
					);
    
    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);
    server.begin();
}

void loop() {
    delay(2000);
    
    WebSerial.print(F("IP address: "));
    WebSerial.println(WiFi.localIP());
    WebSerial.printf("Millis=%lu\n", millis());
    WebSerial.printf("Free heap=[%u]\n", ESP.getFreeHeap());
}
