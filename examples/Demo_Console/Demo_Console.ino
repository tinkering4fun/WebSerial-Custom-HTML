/*
  WebSerial Demo AP - 
  ------
  This example code works for both ESP8266 & ESP32 Microcontrollers
  WebSerial is accessible at http://192.168.4.1/webserial URL.

  Author: Ayush Sharma
  Checkout WebSerial Pro: https://webserial.pro
  ------
  
  Modified by tinkering4fun, January 2024
  Library can support individual HTML pages.
  So one can build a simple UI w/o using the Vue framework.
  Added 'webserial_console.h' to illustrate basic functionality.
  
  The example shows a remote console to send and execute commands
  from a console window.
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

// Comment out the following line to use WebSerial's standard UI
#define CUSTOM_HTML

#ifdef CUSTOM_HTML
// This file contains an example for custom HTML / Javascript,
// may be replaced by your own code
#include "webserial_console.h"
#endif


AsyncWebServer server(80);

const char* ssid = ""; 			// Your WiFi AP SSID 
const char* password = ""; 	// Your WiFi Password


static bool run = false;
static char hostname[32+1] = { "myserver" };
static char mqttTopic[64+1] = { "mysensor/data" };

//----------------------------------------------------------------------
// Webserial message handler callback
//----------------------------------------------------------------------
// Just some silly skeleton for a parameter configuration interface
// via the Webserial console ...
// Of course, this approach is NOT TO BE EXPOSED to the Internet!
void recvMsg(uint8_t *data, size_t len){

	// Ugly hack to detect a new connect
	// The client shall send this trigger at websocket onOpen
	if(!strncmp((const char *)data, "+-+-+-",3)){
		WebSerial.print("\nHint: Use '?' for help\n");
		return;
	}
	
  char *cmd = (char *)malloc(len + 1);
  if (cmd != NULL) {
		// Copy data, ensure ASCIIZ 
	  memcpy(cmd, data, len);
	  cmd[len] = 0;
	
		Serial.printf("Got: %s\n", cmd);

		// Dispatch command
	  WebSerial.printf("\nReceived: %s\n", cmd);
	  bool understood = true;
	  if (!strcmp(cmd, "?")) {
			WebSerial.printf("Commands:\n"
			                 "  param     Show parameters\n"
			                 "  start     Start message loop\n"
			                 "  stop      Stop message loop\n" );
		}
		else if(!strcmp(cmd, "start")) {
			run = true;
		}
		else if(!strcmp(cmd, "stop")) {
			run = false;
		}
		else if(!strcmp(cmd, "param")) {
			WebSerial.printf("  Parameter     Value\n"
											 "  -----------   ---------------\n"
											 "  hostname:     %s\n"
			                 "  mqttTopic:    %s\n"
			                 "\n"
			                 "Use 'set param value' to change parameter\n"
			                 , hostname
			                 , mqttTopic
			                 );
		}
		else {
			char verb[8+1], param[16+1], value[32+1];
			if(sscanf(cmd, "%8s %16s %32s", verb, param, value) != 3)
				understood = false;
			else {
				Serial.printf("c: %s, p: %s, v: %s\n", verb, param, value);
				if(!strcmp(verb, "set")){
					// Set parameter ...
					if(!strcmp(param,"hostname")){
						strncpy(hostname, value, sizeof(hostname));
					}
					else if(!strcmp(param, "mqttTopic")){
						strncpy(mqttTopic, value, sizeof(mqttTopic));
					}
					else
						understood = false;
				}
				else
					understood = false;
			}
		}
		if(!understood){
			WebSerial.printf("Bad command, type '?' for help\n");
		}
		else
			WebSerial.printf("OK\n");
  
		free(cmd);
  } 
  else 
		WebSerial.print("ERROR malloc()\n");

	return;
}

//----------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.print(F("\n--- Webservice Console Demo ---\n"));
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    // WebSerial is accessible at "<IP Address>/webserial" in browser

#ifdef CUSTOM_HTML  
		
    // You may specify your own HTML with begin() ...
    WebSerial.begin(&server, 							// the server
					"/webserial", 									// URL
					WEBSERIAL_CONSOLE_HTML, 				// HTML char stream
					sizeof(WEBSERIAL_CONSOLE_HTML), // size of dto
					false							// unzipped
					);
#else
	// Use standard page 
    WebSerial.begin(&server);
#endif
    
    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);
    server.begin();
}

//----------------------------------------------------------------------
void loop() {
	delay(2000);
	
	if(run){
		WebSerial.print(F("IP address: "));
		WebSerial.println(WiFi.localIP());
		WebSerial.printf("Millis=%lu\n", millis());
		WebSerial.printf("Free heap=[%u]\n", ESP.getFreeHeap());
	}

}
// eof
