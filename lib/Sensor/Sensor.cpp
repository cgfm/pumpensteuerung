#include <Arduino.h>
#include <Sensor.h>

Sensor::Sensor(int pin){
    this->pin=pin;
}

Sensor::Sensor(int pin, String name){
    this->pin=pin;
    this->name=name;
}

int Sensor::getPin(){
    return this->pin;
}

void Sensor::setPin(int pin){
    this->pin=pin;
}

String Sensor::getName(){
    return this->name;
}

void Sensor::setName(String name){
    this->name=name;
}
        
float Sensor::readSensor(){
    return digitalRead(pin);
}

void Sensor::print(){
    Serial.println(name + " at Pin:" + pin); 
};

String Sensor::printWeb(){
  return "<div class=\"left\"><h4 class=\"inline\">" + name + "</h4></div>";
}