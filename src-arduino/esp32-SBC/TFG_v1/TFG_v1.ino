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
const char* ssid = "MOVISTAR_9E73";
const char* password = "bmrpzvM3yQVXzXYPT7qh";
const char* token = "Bearer eyJhbGciOiJIUzUxMiJ9.eyJzdWIiOiJ0ZW5hbnRAdGhpbmdzYm9hcmQub3JnIiwic2NvcGVzIjpbIlRFTkFOVF9BRE1JTiJdLCJ1c2VySWQiOiJlMWQ4NTU0MC1iYzlkLTExZWEtYTI2YS0wMWZmNTZiMDBiYzIiLCJlbmFibGVkIjp0cnVlLCJpc1B1YmxpYyI6ZmFsc2UsInRlbmFudElkIjoiZTBmMDNmMzAtYmM5ZC0xMWVhLWEyNmEtMDFmZjU2YjAwYmMyIiwiY3VzdG9tZXJJZCI6IjEzODE0MDAwLTFkZDItMTFiMi04MDgwLTgwODA4MDgwODA4MCIsImlzcyI6InRoaW5nc2JvYXJkLmlvIiwiaWF0IjoxNTk2MDM1MTE1LCJleHAiOjE2Mjc1NzExMTV9.oGd85JaLhScR2cC5AX6RdcQAJJeq7pl6Hglq4Bk4pFerIikHXEyTSgySdhKliBKLXpr9lwbbdgROF4u8KiY_FA";

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
  for (int i = 0; i < 10; i++) {
    digitalWrite(color, HIGH);
    delay(500);
    digitalWrite(color, LOW);
    delay(500);
  }
}

void led_parpadeo_mezcla(int color, int color1) {  
  for (int i = 0; i < 10; i++) {
     digitalWrite(color, HIGH);
     digitalWrite(color1, HIGH);
     delay(500);
     digitalWrite(color, LOW);
     digitalWrite(color1, LOW);
 
    delay(500);
  }
}
int calcularPeso() {
  float valor_peso = 0;
  valor_peso = analogRead(pesoPin);
  return valor_peso;
}

int calcularHumedadSueloINT() {
  //Devuelva el valor a la inversa 4095 es que no tinee nada de humedad
  //El valor Minimo obtenido es de 1024, lo que significa que esa es la mayor humedad 
  //Se obtiene el valor en tanto por cierto mediante una regla de tres
  //1024              100%
  //valor_humedad       X%
  //(valor_humedad *100/1024)/100
int valor_humedad = 0;
  valor_humedad = analogRead(humedadPinINT);
  Serial.println(valor_humedad);
   int valor = 4095 - valor_humedad;
  valor_humedad = (valor * 100)/3071;
  return valor_humedad;
}

int comprobarAgua() {
  int valor_agua = 0;
  valor_agua = analogRead(aguaPin);
  if (valor_agua > 3500){ valor_agua = 0;}else{ valor_agua = 1;}
  return valor_agua;
}

int calcularHumedadSueloEXT() {
  //Devuelva el valor a la inversa 4095 es que no tinee nada de humedad
  //El valor Minimo obtenido es de 1024, lo que significa que esa es la mayor humedad 
  //Se obtiene el valor en tanto por cierto mediante una regla de tres
  //1024              100%
  //valor_humedad       X%
  //(valor_humedad *100/1024)/100
  int valor_humedad = 0;
  valor_humedad = analogRead(humedadPinINT);
  Serial.print(valor_humedad);
  int valor = 4095 -valor_humedad;
  valor_humedad = (valor * 100)/3962;
  return valor_humedad;
}

int comprobarAgua() {
  int valor_agua = 0;
  valor_agua = analogRead(aguaPin);
  if (valor_agua > 3500){ valor_agua = 0;}else{ valor_agua = 1;}
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
  Serial.print(iaqcore.getCO2PredictionPPM());
   Serial.println(" ppm");
  if (eco2 > 2000) {
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


int sendData(JsonObject& valores) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
      char json_str[1000];
    valores.printTo(json_str, sizeof(json_str));
    http.begin("http://138.4.92.46:8080/api/v1/66WxSzusOoPemMXixHHV/telemetry");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Authorization", token);
    int httpResponseCode = http.POST(json_str); //Send the actual POST request


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

  http.begin("http://138.4.92.46:8080/api/v1/66WxSzusOoPemMXixHHV/attributes");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Authorization", token);

  StaticJsonBuffer<300> JSONBuffer; //Memory pool
  int httpCode = http.GET();                                        //Make the request

  if (httpCode > 0) {
    String payload = http.getString();
    JsonObject& root = jsonBuffer.parseObject(payload);
   // T_DESCONEXION_MAX = root["shared"]["T_DESCONEXION_MAX"];
  }

  http.end();
}

void readActuator() {
  HTTPClient http;
  DynamicJsonBuffer jsonBuffer;

  http.begin("http://138.4.92.46:8080/api/plugins/telemetry/DEVICE/75a6f2f0-c13e-11ea-bd36-25db62324136/values/timeseries");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Authorization", token);

  StaticJsonBuffer<300> JSONBuffer; //Memory pool
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    JsonObject& root = jsonBuffer.parseObject(payload);
    String rele = root["rele"][0]["value"];
    String leds = root["leds"][0]["value"];
    String riegoManual = root["riegoManual"][0]["value"];
    agua = comprobarAgua();
    if(riegoManual == "true"){
      led_parpadeo_mezcla(4,16);
    }
     if(rele == "1" && agua == "0"){
      Serial.print("Actuador rele leido:  ");Serial.println(rele);
      digitalWrite(relePin, HIGH);      
      led_parpadeo(ledAzul);
    }else if(rele == "0"){
      Serial.print("Actuador rele leido:  ");Serial.println(rele);
      digitalWrite(relePin, LOW);
    }   
  } else {
      Serial.print("Error on sending POST Request: ");
      Serial.println(httpResponseCode);
      led_parpadeo_mezcla(4,16);
   } 
  http.end();

}
void loop() {

  // allocate the memory for the document
  DynamicJsonBuffer jsonBuffer;
  // create an empty array
  JsonObject& valores = jsonBuffer.createObject();

  int rele, peso, humedad_int, humedad_ext, co2, agua;
  float luminosidad, temperatura, humedad;
  // rele = comprobarSistema();
  //Se recogen los valores
  peso = calcularPeso();
  humedad_int = calcularHumedadSueloINT();
  humedad = read_humidity();
  co2 = read_co2();
  luminosidad = read_lux();
  temperatura = read_temperature();
  agua = comprobarAgua();

valores["humedad_ambiental"] = humedad;
valores["luz_ambiental"] = luminosidad;
valores["humedad_suelo"] = humedad_int;
valores["sensor_peso"] = peso;
valores["agua"] = agua;
valores["Co2"] = co2;
valores["temperatura_ambiental"] = temperatura;

 valores.prettyPrintTo(Serial);
  sendData(valores);
delay(10000);
readActuator();
  delay(50000);
}
