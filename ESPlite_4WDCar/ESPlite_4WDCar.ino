#define MQTT_MAX_PACKET_SIZE 1024

#include <AuthClient.h>
#include <MicroGear.h>
#include <ArduinoJson.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <MicroGear.h>
#include "timer.hpp"

const char* ssid     = "ESPERT-002";
const char* password = "espertap";


#define APPID        "HelloCMMC"
#define KEY          "ResB8KPliGjZbwL"
#define SECRET       "0U1SeCu3QDEeDIGcEKpRrOVDb"
#define ALIAS        "car"

#define PUBLISH_EVERY_SECS (2*1000)


WiFiClient client;
AuthClient *authclient;
CMMC_Interval timer001;
CMMC_Interval timer002;

// MQTT CONNECTOR CONCEPT
char jsonStrbuffer[1024];
JsonObject *cmmc_root;
JsonObject *d;
JsonObject *cmmc_info;

StaticJsonBuffer<800> jsonRootBuffer;
StaticJsonBuffer<512> jsonDBuffer;


int timer = 0;
MicroGear microgear(client);

#include "_publish.h"
#include "_receive.h"
#include "utils.h"

#define B_OK 13
#define B_UP 12
#define B_DOWN 14
#define L_LED 0
#define C_LED 15
#define R_LED 2

int dimmer = 0;
byte ok_state, up_state, down_state, toggle;

void init_wifi();
void microgear_loop();

void init_hardware() {
  Serial.begin(115200);

  pinMode(L_LED, OUTPUT);
  pinMode(L_LED, OUTPUT);
  pinMode(L_LED, OUTPUT);
  pinMode(C_LED, OUTPUT);
  pinMode(R_LED, OUTPUT);

  digitalWrite(L_LED, LOW);
  digitalWrite(C_LED, LOW);
  digitalWrite(R_LED, LOW);
  Serial.println("Starting...");
}

void setup() {
  _constructor();
  init_hardware();
  init_wifi();
  init_netpie();
}

void loop() {
  microgear_loop();
}

void microgear_loop() {
  if (microgear.connected()) {
    microgear.loop();
    timer001.every_ms(PUBLISH_EVERY_SECS, [&]() {
      _publish();
    });
  }
  else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
}

void init_wifi() {
  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

