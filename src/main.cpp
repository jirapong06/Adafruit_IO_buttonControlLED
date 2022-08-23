#include <Arduino.h>
#include "config.h"

#define LED_PIN 13
AdafruitIO_Feed *button = io.feed("controlBtn");


#include "DHT.h"
#define DHTPIN 18
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float tempSensor; 
float humiditySensor;
AdafruitIO_Feed *tempFeed = io.feed("temp");
AdafruitIO_Feed *humFeed = io.feed("hum");

unsigned long lastSendData = 0;

void handleButton(AdafruitIO_Data *data) {
  Serial.println(data->toString());
  if (data->toString() == "ON") {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT); 
  Serial.begin(115200);
  dht.begin();
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  button->onMessage(handleButton);

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());
  button->get();
  button->save(false);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() - lastSendData >= 1000*60) {
    humiditySensor = dht.readHumidity();
    tempSensor = dht.readTemperature();

    humFeed->save(humiditySensor);
    tempFeed->save(tempSensor);
    
    lastSendData = millis();
  }

  io.run();

}

