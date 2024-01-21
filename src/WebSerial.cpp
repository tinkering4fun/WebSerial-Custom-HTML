#include "WebSerial.h"

void WebSerialClass::begin(AsyncWebServer *server, const char* url,  
							const char* username, const char* password, 
							const char* html, size_t len, bool gzip){
    _server = server;
    
		if(strlen(username) > 0){
        _authRequired = true;
        _username = username;
        _password = password;
    }else{
        _authRequired = false;
        _username = "";
        _password = "";
    }
    
    _ws = new AsyncWebSocket("/webserialws");

    _server->on(url, HTTP_GET, [&, html, len, gzip](AsyncWebServerRequest *request){
			
				if(_authRequired){
					if(!request->authenticate(_username.c_str(), _password.c_str())){
						return request->requestAuthentication();
					}
				}
        // Send Webpage
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", (uint8_t *)html, len);
        if(gzip)
			response->addHeader("Content-Encoding","gzip");
        request->send(response);        
    });

    _ws->onEvent([&](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) -> void {
        if(type == WS_EVT_CONNECT){
            #if defined(DEBUG)
                DEBUG_WEB_SERIAL("Client connection received");
            #endif
        } else if(type == WS_EVT_DISCONNECT){
            #if defined(DEBUG)
                DEBUG_WEB_SERIAL("Client disconnected");
            #endif
        } else if(type == WS_EVT_DATA){
            #if defined(DEBUG)
                DEBUG_WEB_SERIAL("Received Websocket Data");
            #endif
            if(_RecvFunc != NULL){
                _RecvFunc(data, len);
            }
        }
    });

    _server->addHandler(_ws);

    #if defined(WEBSERIAL_DEBUG)
        DEBUG_WEB_SERIAL("Attached AsyncWebServer along with Websockets");
    #endif
}

void WebSerialClass::msgCallback(RecvMsgHandler _recv){
    _RecvFunc = _recv;
}

// Print
size_t WebSerialClass::write(uint8_t m) {
  _ws->textAll((const char *)&(m), 1);
  return(1);
}

size_t WebSerialClass::write(const uint8_t* buffer, size_t size) {
  _ws->textAll((const char *)buffer, size);
  return(size);
}

#if defined(WEBSERIAL_DEBUG)
    void WebSerialClass::DEBUG_WEB_SERIAL(const char* message){
        Serial.println("[WebSerial] "+message);
    }
#endif

WebSerialClass WebSerial;
