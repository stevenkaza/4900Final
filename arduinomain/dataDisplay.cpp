#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include "dataDisplay.h"
#include <string.h>
#include <EEPROM.h>
#define NETWORKNAME "TBPM"
#define NETWORKPASS "donger420"


ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);
//extern Stream * _GlobalStream;  


dataDisplay wifi;



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] Message: %s\n", num, payload);
            if (strcmp((char *)payload,"1")==0){
                USE_SERIAL.print("Turning ON LED\n");

                wifi.enableLED();
            }

            if (strcmp((char *)payload,"0")==0){
                USE_SERIAL.print("Turning OFF LED\n");

                wifi.disableLED();
            }
         

            USE_SERIAL.print("Sending Payload\n");
            USE_SERIAL.print("Recieving Payload\n");

            break;
    }

}


void dataDisplay::setupLED(){
    pinMode(0,OUTPUT);
}

void dataDisplay::flashLED(int duration){
    while (1) {
        wifi.enableLED();
        delay(duration);
        wifi.disableLED();
    }
}

void dataDisplay::enableLED(){
    USE_SERIAL.print("ENABLING");
    digitalWrite(3,HIGH);
}

void dataDisplay::disableLED(){
    USE_SERIAL.print("disabling ");
   
    digitalWrite(3,LOW);
}

void dataDisplay::begin() {
    //USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    //USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    int n = WiFi.scanNetworks();
    
    
    Serial.println("Adding TPBM");

    WiFiMulti.addAP(NETWORKNAME, NETWORKPASS);

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    USE_SERIAL.print("IP address: ");

    // this where our dashboard will be hosted, on this IP 
    USE_SERIAL.println(WiFi.localIP());
    USE_SERIAL.flush();

    // start webSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
    }

    // handle index
    server.on("/", []() {
        // send index.html
        server.send(200, "text/html", "<html><head> <title>ESP Dashboard</title> <meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=yes'> <link rel='stylesheet/less' href='less/styles.less' type='text/css'> <script src='https://ajax.googleapis.com/ajax/libs/jquery/2.2.2/jquery.min.js'></script> <link rel='stylesheet' href='https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.4/themes/smoothness/jquery-ui.css'> <script src='https://ajax.googleapis.com/ajax/libs/jqueryui/1.11.4/jquery-ui.min.js'></script> <script>$(function(){$('#tabs').tabs();}); $(function(){$('#radio').buttonset();}); var connection=new WebSocket('ws://192.168.0.20:81/', ['arduino']); connection.onopen=function(){console.log('opens connection');}; connection.onerror=function(error){console.log('WebSocket Error ', error);}; connection.onmessage=function(e){console.log('Server: ', e.data); connection.send('Time: ' + new Date());}; $(function(){$('#radio1').click(function(){console.log('button ON'); connection.send('1');}); $('#radio2').click(function(){console.log('button OFF'); connection.send('0');});}); </script> <style>.outer{width: 100%; height: 1000px;}.inner{margin: 0 auto; padding: 0 20px; height: 200px;}.indicator{height: 100px; width: 100%; max-width: 100px;}.indicator span{display: block; padding-left: 7px;}.light{width: 50px; height: 50px; border-radius: 50%; display: block;}.on{background-color: green;}.off{background-color: red;}</style></head><body> <div class='headerContainer'> <header> </header> </div><div class='outer outer-1'> <div id='tabs' class='inner inner-1'> <ul> <li><a href='#component1'>LED</a></li><li><a href='#component2'>Ultrasonic</a></li><li><a href='#component3'>Switch</a></li></ul> <div id='component1'> <div> <form> <div id='radio'> <input type='radio' id='radio1' name='radio'> <label for='radio1'>On</label> <input type='radio' id='radio2' name='radio'> <label for='radio2' checked='checked'>Off</label> </div></form> </div></div><div id='component2'>Distance=</div><div id='component3'> <div class='indicator'> <div class='light off'></div><span>OFF</span> </div></div></div></div></body></html>");
       // server.send(200,"text/stylesheet", "color:bold; ")
    });

    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

   // USE_SERIAL.println("shes a anilla make hannah jealous");

}

void dataDisplay::process(){
//  ledHandler();
 // servoHandler();
  //autoHandler();
  //calibrateHandler();
 //sensorNotifier();
  //checkReady();
  wifi.wifiLoop();
 
}
void dataDisplay::wifiLoop() {
    webSocket.loop();
    server.handleClient();
}
