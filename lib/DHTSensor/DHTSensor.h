#ifndef _DHTSENSOR_H
#define _DHTSENSOR_H
#include <Sensor.h>
#include <DHT.h>

class DHTSensor : public Sensor{
    protected:
        int gndPin;
        DHT dht=DHT(1,DHT22);
        float humidity;
        float temperature;
    public:
        DHTSensor(int pin, String name, uint8_t sensortype);
        DHTSensor(int pin, String name, uint8_t sensortype, uint8_t count);
        DHTSensor(int pin, String name, int gndPin, uint8_t sensortype);
        DHTSensor(int pin, String name, int gndPin, uint8_t sensortype, uint8_t count);
        float readSensor();
        float getHumidity();
        float getTemperature();
        DHT getSensor();
        void print();
        String printWeb();
};

#endif