#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>

// WiFi network credentials
const char *ssid = "Palixmajo";
const char *password = "26540mj522";

// HTTP Server Port
const int http_port = 80;

// Red, green, and blue pins for PWM control
// Left RGB Led
const int redPin = 13;     // 13 corresponds to GPIO13
const int greenPin = 12;   // 12 corresponds to GPIO12
const int bluePin = 14;    // 14 corresponds to GPIO14

// Right RGB Led
const int redPin2 = 27;    // 27 corresponds to GPIO27
const int greenPin2 = 26;  // 26 corresponds to GPIO26
const int bluePin2 = 25;   // 25 corresponds to GPIO25

// Setting PWM frequency, channels and bit resolution
const int freq = 5000;

// Channels corresponds to the 16 channels of a PWM signal
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;

const int redChannel2 = 3;
const int greenChannel2 = 4;
const int blueChannel2 = 5;

// Bit resolution 2^8 = 255
const int resolution = 8;

// Globals
AsyncWebServer server(http_port);
WebSocketsServer webSocket = WebSocketsServer(1337);

// Funciones
// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest1(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

void onCSSRequest2(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/styles.css", "text/css");
}

// Callback: send JS files
void onJSRequest1(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/pickr.es5.min.js", "text/javascript");
}

void onJSRequest2(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/script.js", "text/javascript");
}

// Function to encode rgb code
void encodeRGB (uint8_t *payload){
  String rgb,red,green,blue;
  long red_,green_,blue_;
  rgb = (char *)payload;
  Serial.println(rgb);
  red = rgb.substring(1,3);
  Serial.println(red);
  red_ = strtol(red.c_str(),NULL,16);
  Serial.println(red_);
  green = rgb.substring(3,5);
  Serial.println(green);
  green_ = strtol(green.c_str(),NULL,16);
  Serial.println(green_);
  blue = rgb.substring(5);
  Serial.println(blue);
  blue_ = strtol(blue.c_str(),NULL,16);
  Serial.println(blue_);
  ledcWrite(redChannel, red_);
  ledcWrite(greenChannel, green_);
  ledcWrite(blueChannel, blue_);
  ledcWrite(redChannel2, red_);
  ledcWrite(greenChannel2, green_);
  ledcWrite(blueChannel2, blue_);
}

void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{

  // Figure out the type of WebSocket event
  switch (type)
  {

  // Client has disconnected
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", client_num);
    break;

  // New client has connected
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(client_num);
    Serial.printf("[%u] Connection from ", client_num);
    Serial.println(ip.toString());
  }
  break;

  // Handle text messages from client
  case WStype_TEXT:

    // Print out raw message
    Serial.printf("[%u] Received text: %s\n", client_num, payload);
    encodeRGB(payload);
    break;

  // For everything else: do nothing
  case WStype_BIN:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // configure LED PWM functionalitites
  // Left RGB Led
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  // Right RGB Led
  ledcSetup(redChannel2, freq, resolution);
  ledcSetup(greenChannel2, freq, resolution);
  ledcSetup(blueChannel2, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);

  ledcAttachPin(redPin2, redChannel2);
  ledcAttachPin(greenPin2, greenChannel2);
  ledcAttachPin(bluePin2, blueChannel2);

    // Make sure we can read the file system
  if (!SPIFFS.begin())
  {
    Serial.println("Error mounting SPIFFS");
    while (1);
  }

  WiFi.softAP(ssid,password);
  
  // Print our IP address
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("ledcontroller"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide CSS
  server.on("/style.css", HTTP_GET, onCSSRequest1);
  server.on("/styles.css", HTTP_GET, onCSSRequest2);

  // Handle requests for pages that do not exist
  server.on("/pickr.es5.min.js", HTTP_GET, onJSRequest1);
  server.on("/script.js", HTTP_GET, onJSRequest2);

  server.begin();
  MDNS.addService("http", "tcp", http_port);
  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  webSocket.loop();
}