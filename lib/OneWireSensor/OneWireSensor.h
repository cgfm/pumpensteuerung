#ifndef _ONEWIRESENSOR_H
#define _ONEWIRESENSOR_H
#include <Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class OneWireSensor : public Sensor{
    protected:
        OneWire oneWire; 
        DallasTemperature dallastemp;
        float temperature;
    public:
        OneWireSensor(int pin, String name);
        float readSensor();
        float getTemperature();
        void print();
        String printWeb();
};

#endif