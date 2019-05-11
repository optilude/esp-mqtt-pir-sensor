/*

  To use this code you will need the following dependencies: 
  
  - Support for the ESP8266 boards. 
        - You can add it to the board manager by going to File -> Preference and pasting http://arduino.esp8266.com/stable/package_esp8266com_index.json into the Additional Board Managers URL field.
        - Next, download the ESP8266 dependancies by going to Tools -> Board -> Board Manager and searching for ESP8266 and installing it.
  
  - You will also need to download the follow libraries by going to Sketch -> Include Libraries -> Manage Libraries
  
      - Adafruit unified sensor
      - PubSubClient
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// You must create `secrets.h` in this directory. See `secrets.h.template` for an example.

#include "secrets.h"

/**************************** PIN DEFINITIONS ********************************************/

#define PIRPIN    D5

/**************************** SENSOR DEFINITIONS *******************************************/

#define PIR_OFF 1
#define PIR_ON 2

int pirValue;
int pirStatus = PIR_OFF;
String motionStatus = "off";

int calibrationTime = 0;

// #define MQTT_MAX_PACKET_SIZE 512

WiFiClient espClient;
PubSubClient client(espClient);

/********************************** START SETUP*****************************************/
void setup() {

  Serial.begin(115200);

  pinMode(PIRPIN, INPUT);

  Serial.begin(115200);
  delay(10);

  ArduinoOTA.setHostname(SENSORNAME);
  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setPassword((const char *)OTApassword);

  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Starting Node named " + String(SENSORNAME));

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);


  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IPess: ");
  Serial.println(WiFi.localIP());
  
  reconnect();
}




/********************************** START SETUP WIFI*****************************************/
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/********************************** START CALLBACK*****************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  sendState();
}

/********************************** START SEND STATE*****************************************/
void sendState() {
  char buffer[motionStatus.length() + 1];
  motionStatus.toCharArray(buffer, sizeof(buffer));

  Serial.println(buffer);
  client.publish(motion_state_topic, buffer, true);
}


/********************************** START RECONNECT*****************************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(SENSORNAME, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(motion_set_topic);
      sendState();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/********************************** START MAIN LOOP***************************************/
void loop() {

  ArduinoOTA.handle();
  
  if (!client.connected()) {
    // reconnect();
    software_reset();
  }
  client.loop();

  //PIR CODE
  pirValue = digitalRead(PIRPIN); //read state of the PIR sensor

  if (pirValue == LOW && pirStatus != PIR_OFF) {
    motionStatus = "off";
    pirStatus = PIR_OFF;
    sendState();
  } else if (pirValue == HIGH && pirStatus != PIR_ON) {
    motionStatus = "on";
    pirStatus = PIR_ON;
    sendState();
  }
  
  delay(1000);

}

/****reset***/
void software_reset() { // Restarts program from beginning but does not reset the peripherals and registers
  Serial.print("resetting");
  ESP.reset(); 
}
