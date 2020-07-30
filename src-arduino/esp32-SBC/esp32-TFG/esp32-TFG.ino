#include <ArduinoJson.h>
#include <SHTSensor.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include <Wire.h>
#include <VEML7700.h>
#include "iAQcore.h"
#include <MQTT.h>

VEML7700 als;
//Adafruit_VEML7700 als;
iAQcore iaqcore;
SHTSensor sht;
WiFiClient net;
MQTTClient client;
/////////////////////////
//        PINES        //
/////////////////////////
const int humedadPinINT = 36;
const int humedadPinEXT = 39;
const int pesoPin = 37;
const int aguaPin = 38;
const int relePin = 13;

const int ledRojo = 4;
const int ledVerde = 17;
const int ledAzul = 16;

/////////////////////////
//        WIFI        //
/////////////////////////
const char* ssid = "MiFibra-8D32";
const char* password = "xQuo6d69";

unsigned long lastMillis = 0;
//PRUEBAS PARA LAS REGLAS
void led_wifi_connecting(){
   digitalWrite(ledVerde, HIGH);
   digitalWrite(ledRojo, HIGH);
   delay(500);
   digitalWrite(ledRojo, LOW);
   digitalWrite(ledVerde, LOW);
}
void led_wifi_ok(){
   digitalWrite(ledAzul, HIGH);
   digitalWrite(ledVerde, HIGH);
   delay(3000);
   digitalWrite(ledAzul, LOW);
   digitalWrite(ledVerde, LOW);
}

void connect() {
   
   while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    led_wifi_connecting();
  }

  Serial.println("Connected to the WiFi network");
  led_wifi_ok();

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "try", "try")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe('v1/devices/me/rpc/response/+')
  client.publish('v1/devices/me/rpc/request/1',json.dumps({"method": "regla1","params": {"T_RIEGO":"1"}}), 1)
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  client.begin("broker.shiftr.io", net);
  client.onMessage(messageReceived);

  connect();
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }
}
