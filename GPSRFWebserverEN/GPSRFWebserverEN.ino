#include <FS.h>
#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266HTTPUpdateServer.h>
#include <SoftwareSerial.h>


//--- Begin Pin Declarations ---//
const byte HC12RxdPin = 14;                          // "RXD" Pin on HC12
const byte HC12TxdPin = 12;                          // "TXD" Pin on HC12
const byte HC12SetPin = 13;                          // "SET" Pin on HC12
//--- End Pin Declarations ---//

char byteIn;                                        // Temporary variable
String HC12ReadBuffer = "";                         // Read/Write Buffer 1 -- Serial
String SerialReadBuffer = "";                       // Read/Write Buffer 2 -- HC12

char *galt = NULL ;
char *gtime = NULL ;
char *gdate = NULL ;
float glatweb;
float glngweb;
float gspeedweb;
char* GPSSentence;

//float VCC;
//float VccGPS;
//char* VccText;
//char msgBuffer[6];

boolean serialEnd = false;                          // Flag for End of Serial String
boolean HC12End = false;                            // Flag for End of HC12 String
boolean commandMode = false;                        // Send AT commands to remote receivers

const char* host = "TrackerGPSRF";
//const char* hostUpdate = "GPSRFWUpdate";
const char* ssid = "TrackerGPSRF";
const char* password = "123456789";

//--- End variable declarations ---//

// Create Software Serial Ports for HC12 & GPS
// Software Serial ports Rx and Tx are opposite the HC12 Rxd and Txd
SoftwareSerial HC12(HC12TxdPin, HC12RxdPin);


// Define Access point IP
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);
//ESP8266WebServer httpServer(81);
//ESP8266HTTPUpdateServer httpUpdater;
//ADC_MODE(ADC_VCC);


void setup() {

  Serial.begin(9600);  // Open serial port to computer at 9600 Baud
  Serial.setDebugOutput(true);
  WiFi.setAutoConnect(false);
  HC12ReadBuffer.reserve(82);                       // Reserve 82 bytes for message
  SerialReadBuffer.reserve(82);                     // Reserve 82 bytes for message
  //  GPSReadBuffer.reserve(82);                        // Reserve 82 bytes for longest NMEA sentence

  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  ESP.eraseConfig() ;
  WiFi.disconnect() ;
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);  // subnet FF FF FF 00

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();


  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //  MDNS.begin(host);
  //  MDNS.begin(hostUpdate);

  //  httpUpdater.setup(&httpServer);
  //  httpServer.begin();

  //  MDNS.addService("http", "tcp", 81);
  // MDNS.addService("http", "tcp", 80);
  //  Serial.printf("HTTP Update Server is ready! Open http://%s.local:81/update in your browser\n", hostUpdate);

  if (!SPIFFS.begin())
  {
    // Serious problem
    Serial.println("SPIFFS Mount failed");
  } else {
    Serial.println("SPIFFS Mount succesfull");
  }
  server.on("/gps.json", sendGPS);
  server.serveStatic("/index.html", SPIFFS, "/index.html");
  //  server.serveStatic("/", SPIFFS, "/index.html");
  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
  // and check if the file exists


  server.begin();
  Serial.printf("HTTP Web Server ready! Open http://%s.local/index.html in your browser\n", host);
  delay(3000);


  pinMode(HC12SetPin, OUTPUT);                      // Output High for Transparent / Low for Command
  digitalWrite(HC12SetPin, HIGH);                   // Enter Transparent mode
  delay(80);                                        // 80 ms delay before operation per datasheet

  HC12.begin(9600);                                 // Open software serial port to HC12 at 2400 Baud
  //  GPS.begin(9600);                                  // Open software serial port to GPS at 9600 Baud
  delay(5000);
  HC12.listen();                                    // Listen to HC12


}


void loop() {

  while (HC12.available()) {                        // If Arduino's HC12 rx buffer has data
    byteIn = HC12.read();                           // Store each character in byteIn

    HC12ReadBuffer += char(byteIn);                 // Write each character of byteIn to HC12ReadBuffer
    if (byteIn == '\n') {                           // At the end of the line
      HC12End = true;                               // Set HC12End flag to true.
    }
  }

  while (Serial.available()) {                      // If Arduino's computer rx buffer has data
    byteIn = Serial.read();                         // Store each character in byteIn
    SerialReadBuffer += char(byteIn);               // Write each character of byteIn to SerialReadBuffer
    if (byteIn == '\n') {                           // At the end of the line
      serialEnd = true;                             // Set serialEnd flag to true.
    }
  }

  if (serialEnd) {                                  // Check to see if serialEnd flag is true
    if (SerialReadBuffer.startsWith("AT")) {        // Check to see if a command has been sent
      if (SerialReadBuffer.startsWith("AT+B")) {    // If it is a baud change command, delete it immediately
        SerialReadBuffer = "";
        Serial.print("Denied: Changing HC12 Baud does not change Arduino Baudrate");
      }
      HC12.print(SerialReadBuffer);                 // Send local command to remote HC12 before changing settings
      delay(100);                                   //
      digitalWrite(HC12SetPin, LOW);                // If true, enter command mode
      delay(100);                                   // Delay before writing command
      HC12.print(SerialReadBuffer);                 // Send command to HC12
      Serial.print(SerialReadBuffer);               // Send command to serial
      delay(500);                                   // Wait 0.5s for reply
      digitalWrite(HC12SetPin, HIGH);               // Exit command / enter transparent mode
      delay(100);                                   // Delay before proceeding
    }

    HC12.print(SerialReadBuffer);                   // Send text to HC12 to be broadcast
    SerialReadBuffer = "";                          // Clear buffer 2
    serialEnd = false;                              // Reset serialEnd flag
  }

  if (HC12End) {                                    // If HC12End flag is true
    if (HC12ReadBuffer.startsWith("AT")) {          // Check to see if a command was received
      digitalWrite(HC12SetPin, LOW);                // If true, enter command mode
      delay(40);                                    // Delay before writing command
      HC12.print(HC12ReadBuffer);                   // Send incoming command back to HC12
      Serial.println(HC12ReadBuffer);               // Send command to serial
      delay(1000);                                  // Wait 0.5s for reply
      digitalWrite(HC12SetPin, HIGH);               // Exit command / enter transparent mode
      delay(80);                                    // Delay before proceeding
      HC12.println("Remote Command Executed");
    }

    if (HC12ReadBuffer.startsWith("$GPRMC")) {

      Serial.println(HC12ReadBuffer);                 // Send message to screen
      GPSSentence = strdup(HC12ReadBuffer.c_str());
      parse_gpsmessage();
    }
    if (HC12ReadBuffer.startsWith("$GPGGA")) {
      Serial.println(HC12ReadBuffer);                 // Send message to screen
      GPSSentence = strdup(HC12ReadBuffer.c_str());
      parse_gpsmessage();
    }
    HC12ReadBuffer = "";                            // Empty Buffer
    HC12End = false;                                // Reset Flag
  }

  server.handleClient();
  //  httpServer.handleClient();
  //delay(5000);

}


void handleNotFound() {


  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void sendGPS() {

  //  if (VCC > 3.6 and VCC < 3.7) {
  //    VccText = strcat(dtostrf(VCC, 4, 2, msgBuffer), " V. Please change the battery.");
  //
  //  }
  //  else {
  //    VccText = strcat(dtostrf(VCC, 4, 2, msgBuffer), " V.");
  //
  //  }


  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  String json = "{\"Hours\":\"" + String(hr) + "\",";
  json += "\"Minutes\":\"" + String(min % 60) + "\",";
  json += "\"Seconds\":\"" + String(sec % 60) + "\",";
  json += "\"gTime\":\"" + String(gtime) + "\",";
  json += "\"gDate\":\"" + String(gdate) + "\",";
  json += "\"latitude\":\"" + String(glatweb, 6) + "\",";
  json += "\"longitude\":\"" + String(glngweb, 6) + "\",";
  json += "\"altitude\":\"" + String(galt) + "\",";
  json += "\"speed\":\"" + String(gspeedweb) + "\"}";

  server.send(200, "text/html", json);
  Serial.println("json send");
}


void parse_gpsmessage()
{
  float glattemp;
  float glngtemp;
  float gspeedtemp;
  char *gspeed = NULL ;
  char *glat = NULL ;
  char *glng = NULL ;
  char gns = 0 ;
  char gvalid = 0 ;
  char gew = 0 ;
  char* token;
  char delim[] = ",";

  int gps_message_type;

  token = strtok(GPSSentence, delim);
  if (strcmp(token, "$GPRMC") == 0) {
    gps_message_type = 1;

  }
  else if (strcmp(token, "$GPGGA") == 0) {
    gps_message_type = 2;
  }

  switch (gps_message_type)
  {
    case 1:
      //Get Data from GPRMS message
      gtime = strtok(NULL, delim);
      gvalid = *strtok(NULL, delim);
      if (gvalid == 'A') {
        glat = strtok(NULL, delim);
        gns = *strtok(NULL, delim);
        glng = strtok(NULL, delim);
        gew = *strtok(NULL, delim);
        gspeed = strtok(NULL, delim);
        strtok(NULL, delim);
        gdate = strtok(NULL, delim);

        //Convert Data
        glattemp = (String(glat).substring(0, 2).toFloat() + (String(glat).substring(2).toFloat() / 60));
        glngtemp = (String(glng).substring(0, 3).toFloat() + (String(glng).substring(3).toFloat() / 60));
        if (gns == 'S') glattemp = -1 * glattemp;
        if (gew == 'W') glngtemp = -1 * glngtemp;
        gspeedtemp = String(gspeed).toFloat() * 0.514444;
        gspeedweb = gspeedtemp;
        glngweb = glngtemp;
        glatweb = glattemp;
      }
      break;
    case 2:
      //Get Data from GPGGA message
      strtok(NULL, delim);
      glat = strtok(NULL, delim);
      gns = *strtok(NULL, delim);
      glng = strtok(NULL, delim);
      gew = *strtok(NULL, delim);
      gvalid = *strtok(NULL, delim);
      if (gvalid != '0') {
        strtok(NULL, delim);
        strtok(NULL, delim);
        galt = strtok(NULL, delim);
        //Convert Data
        glattemp = (String(glat).substring(0, 2).toFloat() + (String(glat).substring(2).toFloat() / 60));
        glngtemp = (String(glng).substring(0, 3).toFloat() + (String(glng).substring(3).toFloat() / 60));
        if (gns == 'S') glattemp = -1 * glattemp;
        if (gew == 'W') glngtemp = -1 * glngtemp;
        glngweb = glngtemp;
        glatweb = glattemp;
      }
      break;
    default:
      Serial.println("GPS PARSE ERROR");
      break;

  }

  strcpy(GPSSentence, "");


}

