#include <ArduinoJson.h>
#include <SHTSensor.h>
#include "WiFi.h"
#include "HTTPClient.h"
#include <Wire.h>
#include <VEML7700.h>
#include "iAQcore.h"
#include <MQTT.h>

//inicializacion de los objetos de i2c
VEML7700 als;
//Adafruit_VEML7700 als;
iAQcore iaqcore;
SHTSensor sht;

//Definicion de los pines
const int humedadPinINT = 36;
const int humedadPinEXT = 39;
const int pesoPin = 37;
const int aguaPin = 38;
const int relePin = 13;
const int ledRojo = 4;
const int ledVerde = 17;
const int ledAzul = 16;

//Parametros para el WiFi
const char* ssid = "MiFibra-8D32";
const char* password = "xQuo6d69";
const char* token = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ0ZW5hbnRAdGhpbmdzYm9hcmQub3JnIiwic2NvcGVzIjpbIlRFTkFOVF9BRE1JTiJdLCJ1c2VySWQiOiJiODVmNmM3MC00OGVhLTExZWEtODkyNC0zNzNlZGUyZjA2ZTkiLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiYjdiOTQxNjAtNDhlYS0xMWVhLTg5MjQtMzczZWRlMmYwNmU5IiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCIsImlzcyI6InRoaW5nc2JvYXJkLmlvIiwiaWF0IjoxNTgzMDY5NTIyLCJleHAiOjE2MTQ2MDU1MjJ9.wph2pUmoaqDm20lIz-XiM39i_CQmDY4kZHt1juvsFMxMCMqfzVWFm15SKVDfhw9mR0EIDEqLr3xXqL84WdUY5w";
//const char* token = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJmamdhcmNpYS5hbHZhcmV6QGhvdG1haWwuY29tIiwic2NvcGVzIjpbIlRFTkFOVF9BRE1JTiJdLCJ1c2VySWQiOiJkNjA2YWU1MC1lYWQwLTExZTktYmM3Mi1lOWQyMzA2NTUzOTYiLCJmaXJzdE5hbWUiOiJGcmFuY2lzY28gSmF2aWVyIiwibGFzdE5hbWUiOiJHYXJjaWEgQWx2YXJleiIsImVuYWJsZWQiOnRydWUsInByaXZhY3lQb2xpY3lBY2NlcHRlZCI6dHJ1ZSwiaXNQdWJsaWMiOmZhbHNlLCJ0ZW5hbnRJZCI6ImQ1ODA4ZTYwLWVhZDAtMTFlOS1iYzcyLWU5ZDIzMDY1NTM5NiIsImN1c3RvbWVySWQiOiIxMzgxNDAwMC0xZGQyLTExYjItODA4MC04MDgwODA4MDgwODAiLCJpc3MiOiJ0aGluZ3Nib2FyZC5pbyIsImlhdCI6MTU4NDEyMzkwNCwiZXhwIjoxNTg1OTIzOTA0fQ.S0S9-cVCIW78eQ3gaDVAQXn3T_z4oVhhaJC3blBooJMjsEmV7-36MYyShwVB2nH_ZDjT24J9wFbbnXTJ10dg2A";

int co2OK = 1;
int humedadOK = 1;
const char* T_DESCONEXION_MAX;
float T_Desconexion = 0;
boolean riegoManual = false;

void setup() {
  Serial.begin(115200);
  pinMode(humedadPinINT, INPUT);
  pinMode(humedadPinEXT, INPUT);
  pinMode(pesoPin, INPUT);
  pinMode(aguaPin, INPUT);
  pinMode(relePin, OUTPUT);

  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAzul, OUTPUT);


  Wire.begin(21, 22);
  als.begin();

  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }

  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    led_wifi_connecting();
  }

  Serial.println("Connected to the WiFi network");
  led_wifi_ok();
}

void led_wifi_ok() {
  digitalWrite(ledAzul, HIGH);
  digitalWrite(ledVerde, HIGH);
  delay(3000);
  digitalWrite(ledAzul, LOW);
  digitalWrite(ledVerde, LOW);
}

void led_wifi_connecting() {
  digitalWrite(ledVerde, HIGH);
  digitalWrite(ledRojo, HIGH);
  delay(500);
  digitalWrite(ledRojo, LOW);
  digitalWrite(ledVerde, LOW);
}


void led_parpadeo(int color) {
  //digitalWrite(ledRojo, LOW);
  //digitalWrite(ledVerde, LOW);
  for (int i = 0; i < 10; i++) {
    digitalWrite(color, HIGH);
    delay(500);
    digitalWrite(color, LOW);
    Serial.println(i);
    delay(500);
  }
}
int calcularPeso() {
  float valor_peso = 0;
  valor_peso = analogRead(pesoPin);
  return valor_peso;
}

int calcularHumedadSueloINT() {
  int valor_humedad = 0;
  valor_humedad = analogRead(humedadPinINT);
  valor_humedad = 1024 - (valor_humedad / 4);
  return valor_humedad;
}

int calcularHumedadSueloEXT() {
  int valor_humedad = 0;
  valor_humedad = analogRead(humedadPinINT);
  valor_humedad = 1024 - (valor_humedad / 4);
  return valor_humedad;
}

int comprobarAgua() {
  int valor_agua = 0;
  valor_agua = analogRead(aguaPin);
  if (valor_agua > 3500) valor_agua = 0;
  return valor_agua;
}

float read_lux() {
  float luxis;
  als.getALSLux(luxis);
  return luxis;
}

int read_co2() {
  uint16_t eco2;
  uint16_t stat;
  uint32_t resist;
  uint16_t etvoc;
  iaqcore.read(&eco2, &stat, &resist, &etvoc);
  if (eco2 > 30000) {
    eco2 = 449;
  }
  return eco2;
}

float read_temperature() {
  float temperature;
  if (sht.readSample()) {
    temperature = sht.getTemperature();
  } else {
    temperature = 0;
  }
  return temperature;
}

float read_humidity() {
  float humidity;
  if (sht.readSample()) {
    humidity = sht.getHumidity();
  } else {
    humidity = 0;
  }
  return humidity;
}

void activarRele() {
  digitalWrite(relePin, HIGH);
}

void desactivarRele() {
  digitalWrite(relePin, LOW);
}


int sendData(String name_value, float value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://138.4.92.46:8080/api/v1/5wzHzcmXmcvE0yk6leHq/telemetry");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Authorization", token);
    int httpResponseCode = http.POST("{" + name_value + ":" + value + "}"); //Send the actual POST request


    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST Request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    return httpResponseCode;
  } else {
    Serial.println("Error in WiFi connection");
    return 0;
  }
}

void readAttributes() {
  HTTPClient http;
  DynamicJsonBuffer jsonBuffer;
  
  http.begin("http://138.4.92.46:8080/api/v1/5wzHzcmXmcvE0yk6leHq/attributes");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Authorization", token);

  StaticJsonBuffer<300> JSONBuffer; //Memory pool
  int httpCode = http.GET();                                        //Make the request

  if (httpCode > 0) {
    String payload = http.getString();
    JsonObject& root = jsonBuffer.parseObject(payload);
    T_DESCONEXION_MAX = root["shared"]["T_DESCONEXION_MAX"];
  } 
  
  http.end();
}

void readActuator() {
  HTTPClient http;
  DynamicJsonBuffer jsonBuffer;

  http.begin("http://138.4.92.46:8080/api/plugins/telemetry/DEVICE/e6441210-5a6c-11ea-87a7-e3079b83229a/values/timeseries");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Authorization", token);

  StaticJsonBuffer<300> JSONBuffer; //Memory pool
  int httpCode = http.GET(); 
  
  if (httpCode > 0) {
    T_Desconexion = 0;
    String payload = http.getString();
    JsonObject& root = jsonBuffer.parseObject(payload);
    String rele = root["rele"][0]["value"];
    String leds = root["leds"][0]["value"];
  } else if (T_Desconexion > atof(T_DESCONEXION_MAX)) {
    Serial.println("Pasando a Riego Manual");
    led_parpadeo(4);//4 color para leds rojos
    riegoManual = true;
  } else {
    T_Desconexion = T_Desconexion + 1;
  }
  http.end();

}
void loop() {
  /*
    int rele, peso, humedad_int, humedad_ext, co2, agua;
    float luminosidad, temperatura, humedad;
    rele = comprobarSistema();
    //Enviamos Valores
    peso = calcularPeso();
    send_data("peso", peso);
    humedad_int = calcularHumedadSueloINT();
    send_data("humedad_inferior", humedad_int);
    humedad_ext = calcularHumedadSueloEXT();
    send_data("humedad_superior", humedad_ext);
    humedad = read_humidity();
    send_data("humedad_ambiental", humedad);
    co2 = read_co2();
    send_data("co2", co2);
    luminosidad = read_lux();
    send_data("luz_ambiental", luminosidad);
    temperatura = read_temperature();
    send_data("temperatura_ambiental", temperatura);
    agua = comprobarAgua();
    send_data("agua", agua);

    //send_data("rele", rele);
  */
  readActuator();
  delay(5000);
}
