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

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2); 

unsigned long lastSendData = 0;
unsigned long sentCounter = 0;

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
  lcd.init();
  lcd.backlight();
  dht.begin();

  Serial.print("Connecting to Adafruit IO");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting.");

  io.connect();
  button->onMessage(handleButton);

  int attemptCount = 0;
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    lcd.setCursor(0,1);
    lcd.print("Attempt    ");
    lcd.setCursor(8, 1);
    lcd.print(++attemptCount);
    delay(500);
  }
  attemptCount = 0;

  Serial.println();
  Serial.println(io.statusText());
  button->get();
  button->save(false);    // Set switch to off

  lcd.clear();

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() - lastSendData >= 1000*60) {
    humiditySensor = dht.readHumidity();
    tempSensor = dht.readTemperature();

    humFeed->save(humiditySensor);
    tempFeed->save(tempSensor);
    
    Serial.println("Sent Data!");
    lcd.setCursor(0,0);
    lcd.print("Sent Data      ");
    lcd.setCursor(10,0);
    lcd.print(++sentCounter);

    lastSendData = millis();
  }

  io.run();

}

