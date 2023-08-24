#include "lib_WFM.h"

#include <WiFiClient.h>
//#include <ESP8266WebServer.h>

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <FS.h>
#include <SPIFFS.h>

#include "index.h" //Our HTML webpage contents with javascripts
//  <script src="https://code.highcharts.com/highcharts.js"></script>
//<script src="/highcharts.js"></script>
//#define LED 2  //On board LED
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


 


//SSID and Password of your WiFi router
//const char* ssid = "Andrea";
//const char* password = "pinimini";
#if defined(ESP8266)
ESP8266WebServer server(80); //Server on port 80
#else
WebServer server(80); //Server on port 80
#endif

//PWM
int salCh1 = 1;
int freq = 5000;
int resol = 12;
const int pwmSalida = 15;

const int analogOutPin = 11; // Analog output pin 
extern int tempValue = 0;
long int ts = millis();


//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleADC() {

 String adcValue = String(tempValue);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}
//==============================================================
//                  SETUP
//==============================================================





void setup(void){
  conectaWiFi();
  SPIFFS.begin();
  //Serial.begin(115200);
  mlx.begin();
   
   ledcSetup(salCh1,freq,resol);
  ledcAttachPin(pwmSalida,salCh1);
 // WiFi.begin(ssid, password);     //Connect to your WiFi router
 // Serial.println("");

  //Onboard LED port Direction output
  //pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
 // Serial.println("");
 // Serial.print("Connected to ");
 // Serial.println(ssid);
 // Serial.print("IP address: ");
 // Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/temp", handleADC); //This page is called by java Script AJAX
  server.serveStatic("/highcharts.js", SPIFFS, "/highcharts.js");
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  if ( millis() > ts) {
    
    tempValue = (0.8 * tempValue + 0.2 * mlx.readObjectTempC()) ;
    //tempValue = mlx.readObjectTempC();
   
    int y = map(tempValue, -20, 235, 0, 4095);
    ledcWrite(salCh1,y);
    
    ts = millis() + 500;   
  }
  server.handleClient();          //Handle client requests
  
  
}
