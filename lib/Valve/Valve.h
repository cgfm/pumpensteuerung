#ifndef _VALVE_H
#define _VALVE_H
#include <arduino.h>

class Valve{
    protected:
        String name;
        int pin;
        unsigned long openTime=0;
        unsigned long emergencyOff=3600000;
        bool state=false;
        bool openState=true;
    public:
        static Valve* getValveFromID(Valve* valves[],int valveID){
            int i = 0;
            while(valves[i]!=NULL){
                if(valves[i]->getPin()==valveID){
                    return valves[i];
                }
                i++;
            }
            return NULL;
        }

        Valve(String name, int pin);
        Valve(String name, int pin, bool closedState);
        Valve(String name, int pin, bool closedState, unsigned long emergencyOff);
        Valve(String name, int pin, bool state, bool openState);
        Valve(String name, int pin, bool state, bool openState, unsigned long emergencyOff);
        void setName(String name);
        String getName() const;
        void setPin(int pin);
        int getPin() const;
        void setState(bool state);
        void setState(char* state);
        bool getState() const;
        bool getOpenState() const;
        void setOpenTime(unsigned long openTime);
        void setEmergencyoff(unsigned long emergencyoff);
        bool isOpen() const;
        unsigned long getOpenTime() const;
        unsigned long getEmergencyOff() const;
        bool checkValveEmergencyState();
        void print();
        String printWeb();
};

#endif