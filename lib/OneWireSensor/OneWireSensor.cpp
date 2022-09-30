#include <OneWireSensor.h>
#include <Arduino.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <iostream>

#ifndef DEBUG
#define DEBUG false
#endif

OneWireSensor::OneWireSensor(int pin, String name) : Sensor(pin, name){
  oneWire = OneWire(pin); 
  dallastemp = DallasTemperature(&oneWire);
  dallastemp.begin();
}

float OneWireSensor::readSensor(){
  dallastemp.requestTemperatures();
  temperature = dallastemp.getTempCByIndex(0);
  return temperature;
}

float OneWireSensor::getTemperature(){
  return temperature;
}

void OneWireSensor::print(){
    std::cout << "OneWire " << name.c_str() << " Pin:" << pin << " - Temperature: " << temperature << "°C" << std::endl; 
};

String OneWireSensor::printWeb(){
  return "<div class=\"left\"><h4 class=\"inline\">" + name + "</h4><span id=\"onewire_temp\" class=\"value\">" + String(temperature) + "</span> &deg;C</div>";
}