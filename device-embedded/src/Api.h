/*
  This ist the API Plugin.
  Its a prototype for a plugin to the main class of modular.
  The plugin is generic, only the methods handlePOST and handleGET must
  be implemented locally
*/

#ifndef api_h
#define api_h

#include <WiFi101.h>
#include <WiFiConfig.h>
// Thanks to : https://github.com/apolukhin/html_inside_cpp
//#include <webapp/html_begin.pp> // header from this repo
#include <webapp/index.html>
//#include <webapp/html_end.pp>   // header from this repo
#include <SD.h>

#define FILE_BUF 50;
#define RESPONSE_DEFAULT_SUCCESS "{ \"jsonroc\" : \"2.0\", \"result\" : true }"
#define RESPONSE_NOT_FOUND "{ \"jsonroc\" : \"2.0\", \"error\" : \"Method not implemented.\" }"

// Declarations for WiFi API
WiFiServer* server;
WiFiClient client;
int status = WL_IDLE_STATUS;
bool staticAvailable = false;

enum HttpStatus {
  HTTP_OK,
  HTTP_NOT_FOUND
};

// Declare Functions for API
String handleRequest(String req);
void printHeaders();
void start();
void stop();
void setJobState(SystemJobState job);
void teachEmptyStack();
void teachAddOpperation(ChassisOperations op);
void teachRestart();
void printDirectory(File dir, int numTabs);

String handlePOST(String url, String content);
String handleGET(String url, String params);
//////////////////////////////////////////////////


// Intitalize Wifi API and Webserver
void setupApi(int8_t cs, int8_t irq, int8_t rst, int8_t en, int port) {
  // Setup the WiFi Connection
  WiFi.setPins(cs, irq, rst, en);
  server = new WiFiServer(port);

  // Here I assuem, that SD has beein initialized, not sure, how to determine this properly
  if (!SD.exists("/www-root")) {
    Serial.println(F("No static http content"));
    staticAvailable = false;
  } else {
    staticAvailable = true;
  }
  // Connect to WiFi or create one
  int cc = 0;
  while (status != WL_CONNECTED || status == WL_AP_LISTENING) {
    if (cc > 0) {
      Serial.print("Create new Accespoint: ");
      Serial.println(ssid_igel);
      status = WiFi.beginAP(ssid_igel);
       if (status != WL_AP_LISTENING) {
         Serial.println("Failed to create Accesspoint");
       }
       break;
    }
    Serial.print("Attempting to connect to SSID (attempt  ");
    Serial.print(cc);
    Serial.print("): ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);
    // Wait 10 seconds for connection:
    delay(1000);
    cc++;
  }

  Serial.println("WiFi connected");
  server->begin();
  Serial.println("Web Server started");
  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

// Loop for API
void loopApi() {
  if (!client) {
    client = server->available();
  }
  else {
    if (client.available()) {
      // read request
      String req = client.readString();
      //handle Request
      String response = handleRequest(req);
      // send response
      client.print(response.c_str());
      client.stop();
    }
  }
}

void printJSONHeaders() {
  //ToDo: Set proper Headers here
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/json");
  client.println("");
}

void printHTMLHeaders(HttpStatus status = HTTP_OK) {
  //ToDo: Set proper Headers here
  switch (status) {
    case HTTP_OK :
      client.println("HTTP/1.1 200 OK");
      break;
    case HTTP_NOT_FOUND :
      client.println("HTTP/1.1 404 Not Found");
      break;
  }
  client.println("Content-Type: text/html");
  client.println("");
}

// General
String handleRequest(String req) {
  debugLn("New Request: ");
  String method = req.substring(0,req.indexOf("/")-1);
  String fullUrl = req.substring(req.indexOf("/"), req.indexOf("HTTP")-1);
  debugLn(fullUrl);
  String url = fullUrl.substring(0, fullUrl.indexOf("?"));
  String params = fullUrl.substring(fullUrl.indexOf("?")+1);
  if (method == "POST") {
    //Serial.println(req);
    String content = req.substring(req.indexOf("\r\n\r\n"));
    //Serial.println(content);
    return handlePOST(url, content);
  } else {
    // for now, treat all other requests like GET requests
    return handleGET(url, params);
  }
}

// Implementation of API handler
String handlePOST(String url, String content) {
  if (url == "/setWalkingPattern") {
    size_t bufferSize = 3*JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(4);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(content);
  }
  return RESPONSE_DEFAULT_SUCCESS;
}

String handleGET(String url, String params) {
  Serial.println(url);
  if (url == "/state") {
    printJSONHeaders();
    return RESPONSE_NOT_FOUND;
  }
  if (url == "/start") {
    start();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS  ;
  }
  if (url == "/stop") {
    stop();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }
  if (url == "/mode/rc") {
    setJobState(REMOTE_CONTROL);
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }
  if (url == "/mode/auto") {
    setJobState(AUTONOMOUS);
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }
  if (url == "/mode/teach") {
    setJobState(TEACH);
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/rc/stop") {
    chassis->stop();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/rc/forward") {
    chassis->steer(STEER_STRAIGHT);
    chassis->forward();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/rc/right") {
    chassis->steer(STEER_RIGHT);
    chassis->forward();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/rc/left") {
    chassis->steer(STEER_LEFT);
    chassis->forward();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/teach/empty") {
    teachEmptyStack();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/teach/add") {
    // TODO: Read Params
    teachAddOpperation(STEP_FORE);
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/teach/restart") {
    teachRestart();
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  if (url == "/voice/play") {
    // Implement a proper param parser later on handleGET level
    int valueStart = params.indexOf("=")+1;
    const char* fileName = strdup(params.substring(valueStart).c_str());
    debugLn(fileName);
    voice->play(fileName);
    printJSONHeaders();
    return RESPONSE_DEFAULT_SUCCESS;
  }

  // for all other GET routes, we assume static http that is stored on the SD Card
  if (url == "/" || url =="/index.html" || url == "") {
    url = "/index.htm";
  }
  String fileName = WWW_ROOT + url;

  debug("Static request for file: ");
  debugLn(fileName);
  File content = SD.open(fileName.c_str());


  if (content) {
    debugLn("File found");
    printHTMLHeaders();
    while (content.available()) {
      Serial.print(content.read());
    }
    content.close();
  }
  else {
    printHTMLHeaders(HTTP_NOT_FOUND);
    File root = SD.open("/");
    printDirectory(root, 0);
  }
  return "";
}

void printDirectory(File dir, int numTabs) {
  // Begin at the start of the directory
  dir.rewindDirectory();

  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');   // we'll have a nice indentation
     }
     // Print the 8.3 name
     Serial.print(entry.name());
     // Recurse for directories, otherwise print the file size
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

#endif
