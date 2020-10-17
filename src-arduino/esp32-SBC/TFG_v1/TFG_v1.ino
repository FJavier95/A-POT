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
const char* ssid = "";
const char* password = "";
const char* token = "";
const char* T_DESCONEXION_MAX;
float T_Desconexion = 0;
float T_MUESTREO;
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
  Serial.print("Humedad del suelo analogica:  "); Serial.println(valor_humedad);
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

    http.begin("http://138.4.92.46:8080/api/v1/66WxSzusOoPemMXixHHV/telemetry");
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
    Serial.print("Actuador rele leido:  ");Serial.println(rele);
    Serial.print("Actuador leds leido:  ");Serial.println(leds);
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

  int rele, peso, humedad_int, humedad_ext, co2, agua;
  float luminosidad, temperatura, humedad;
  readActuator();
  // rele = comprobarSistema();
  //Enviamos Valores
  peso = calcularPeso();
  Serial.print("Peso: "); Serial.println(peso);
  sendData("peso", peso);
  humedad_int = calcularHumedadSueloINT();
  Serial.print("Humedad inferior: "  ); Serial.println(humedad_int);
  sendData("humedad_inferior", humedad_int);
  humedad = read_humidity();
  Serial.print("Humedad ambiental: "  ); Serial.println(humedad);
  sendData("humedad_ambiental", humedad);
  co2 = read_co2();
  Serial.print("Calidad: " );  Serial.println(co2)  ;
  sendData("Co2", co2);
  luminosidad = read_lux();
  Serial.print("Luminosidad: "  );  Serial.println(luminosidad) ;
  sendData("luz_ambiental", luminosidad);
  temperatura = read_temperature();
  Serial.print("Temperatura: "  ); Serial.println(temperatura)  ;
  sendData("temperatura_ambiental", temperatura);
  agua = comprobarAgua();
  Serial.print("Agua: "  );  Serial.println(agua);
  sendData("agua", agua);
  Serial.print("Rele: " ); Serial.println(rele);
  //send_data("rele", rele);


  delay(50000);
}
