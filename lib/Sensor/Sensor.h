#ifndef _SENSOR_H
#define _SENSOR_H
#include <Arduino.h>

class Sensor{
    protected:
        int pin;
        String name;

    public:
        Sensor(int pin);
        Sensor(int pin, String name);
        int getPin();
        void setPin(int pin);
        String getName();
        void setName(String name);
        
        virtual float readSensor();
        virtual void print();
        virtual String printWeb();
};

#endif