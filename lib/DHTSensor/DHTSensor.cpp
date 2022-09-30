#include <DHTSensor.h>
#include <Arduino.h>
#include <DHT.h>

#ifndef DEBUG
#define DEBUG false
#endif

DHTSensor::DHTSensor(int pin, String name, uint8_t sensortype) : Sensor(pin, name){
  this->dht = DHT(pin, sensortype); //// Initialize DHT sensor for normal 16mhz Arduino
  
  pinMode(pin, INPUT_PULLUP);
  this->dht.begin();
}

DHTSensor::DHTSensor(int pin, String name, uint8_t sensortype, uint8_t count) : Sensor(pin, name){
  this->dht = DHT(pin, sensortype, count); //// Initialize DHT sensor for normal 16mhz Arduino
  
  pinMode(pin, INPUT_PULLUP);
  this->dht.begin();
}

DHTSensor::DHTSensor(int pin, String name, int gndPin, uint8_t sensortype) : Sensor(pin, name){
  this->dht = DHT(pin, sensortype); //// Initialize DHT sensor for normal 16mhz Arduino
  this->gndPin=gndPin;
  
  pinMode(pin, INPUT_PULLUP);
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin,HIGH);
  this->dht.begin();
  digitalWrite(gndPin,LOW);
}

DHTSensor::DHTSensor(int pin, String name, int gndPin, uint8_t sensortype, uint8_t count) : Sensor(pin, name){
  this->dht = DHT(pin, sensortype, count); //// Initialize DHT sensor for normal 16mhz Arduino
  this->gndPin=gndPin;
  
  pinMode(pin, INPUT_PULLUP);
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin,HIGH);
  this->dht.begin();
  digitalWrite(gndPin,LOW);
}

float DHTSensor::readSensor(){
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperature = dht.readTemperature();
  return temperature;
}

float DHTSensor::getHumidity(){
  return humidity;
}

float DHTSensor::getTemperature(){
  return temperature;
}

DHT DHTSensor::getSensor(){
  return dht;
}

void DHTSensor::print(){
    Serial.println("DHTSensor " + name + " at Pin:" + pin + " - Temperature: " + temperature + "°C, Humidity: " + humidity + "%"); 
};

String DHTSensor::printWeb(){
  return "<div class=\"left\"><h4 class=\"inline\">" + name + "</h4><span id=\"dht_t\" class=\"value\">" + String(temperature) + "</span> &deg;C / <span id=\"dht_h\" class=\"value\">" + String(humidity) + "</span>  &#37;</div>";
}