#include <CapacitiveMoistureSensor.h>
#include <Arduino.h>
#include <iostream>

#ifndef DEBUG
#define DEBUG false
#endif

CapacitiveMoistureSensor::CapacitiveMoistureSensor(int pin, String name, int minVal, int maxVal) : Sensor(pin, name){
    this->minVal=minVal;
    this->maxVal=maxVal;
    
    pinMode(pin, INPUT);
    
    this->stepsize = (maxVal - minVal);
    this->stepsize /= 100;
}

float CapacitiveMoistureSensor::readSensor(){
  float read = analogRead(pin);

  read -= this->minVal;
  read /= this->stepsize;
  if(read>this->readErrorBorder){
    return -1;
  }
  return read;
}

float CapacitiveMoistureSensor::readSensorRaw(){
  return analogRead(pin);
}

int CapacitiveMoistureSensor::getMinVal(){
    return this->minVal;
};
int CapacitiveMoistureSensor::getMaxVal(){
    return this->maxVal;
};
float CapacitiveMoistureSensor::getStepSize(){
    return this->stepsize;
};
int CapacitiveMoistureSensor::getReadErrorBorder(){
    return this->readErrorBorder;
};
void CapacitiveMoistureSensor::setMinVal(int minVal){
    this->minVal=minVal;
};
void CapacitiveMoistureSensor::setMaxVal(int maxVal){
    this->maxVal=maxVal;
};
void CapacitiveMoistureSensor::setReadErrorBorder(int readErrorBorder){
    this->readErrorBorder=readErrorBorder;
};

void CapacitiveMoistureSensor::print(){}

String CapacitiveMoistureSensor::printWeb(){
  return "<div class=\"left\"><h4 class=\"inline\">" + name + "</h4><span id=\"moisture\" class=\"value\">" + String(readSensor(), 2) + "</span> &#37;</div>";
}