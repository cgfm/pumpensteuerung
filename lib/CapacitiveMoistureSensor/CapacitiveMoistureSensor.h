#ifndef _CAPPACITIVEMOISTURESENSOR_H
#define _CAPPACITIVEMOISTURESENSOR_H
#include <Sensor.h>

class CapacitiveMoistureSensor : public Sensor{
    protected:
        int minVal;
        int maxVal;
        float stepsize;
        int readErrorBorder=105;

    public:
        CapacitiveMoistureSensor(int pin, String name, int minVal, int maxVal);
        float readSensor();
        float readSensorRaw();
        int getMinVal();
        int getMaxVal();
        float getStepSize();
        int getReadErrorBorder();
        void setMinVal(int minVal);
        void setMaxVal(int maxVal);
        void setReadErrorBorder(int readErrorBorder);
        void print();
        String printWeb();
};

#endif