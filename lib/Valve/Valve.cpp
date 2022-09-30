#include <valve.h>

Valve::Valve(String name, int pin){
    this->name=name;
    this->pin=pin;
    pinMode(pin, OUTPUT);
};
Valve::Valve(String name, int pin, bool closedState){
    this->name=name;
    this->pin=pin;
    this->state=closedState;
    this->openState=!closedState;

    pinMode(pin, OUTPUT);
    digitalWrite(pin,closedState);
};
Valve::Valve(String name, int pin, bool closedState, unsigned long emergencyOff){
    this->name=name;
    this->pin=pin;
    this->state=closedState;
    this->openState=!closedState;
    this->emergencyOff = emergencyOff;

    pinMode(pin, OUTPUT);
    digitalWrite(pin,closedState);
};
Valve::Valve(String name, int pin, bool state, bool openState){
    this->name=name;
    this->pin=pin;
    this->state=state;
    this->openState=openState;

    pinMode(pin, OUTPUT);
    digitalWrite(pin,state);
};
Valve::Valve(String name, int pin, bool state, bool openState, unsigned long emergencyOff){
    this->name=name;
    this->pin=pin;
    this->state=state;
    this->openState=openState;
    this->emergencyOff = emergencyOff;

    pinMode(pin, OUTPUT);
    digitalWrite(pin,state);
};

void Valve::setName(String name){
    this->name=name;
};
String Valve::getName() const {
    return this->name;
};

void Valve::setPin(int pin){
    this->pin=pin;
    pinMode(pin, OUTPUT);
};
int Valve::getPin() const {
    return this->pin;
};

void Valve::setState(bool state){
    this->state=state;
    digitalWrite(this->pin, state);
};
void Valve::setState(char* state){
    if(String(state).equals("ON")){
        this->setState(this->openState);
    }else{
        this->setState(!this->openState);
    }
};
bool Valve::getState() const {
    return this->state;
};

bool Valve::isOpen() const {
    return this->state==this->openState;
};
bool Valve::getOpenState() const {
    return this->openState;
};

void Valve::setOpenTime(unsigned long openTime){
    this->openTime=openTime;
};
unsigned long Valve::getOpenTime() const {
    return this->openTime;
};

void Valve::setEmergencyoff(unsigned long emergencyOff){
    this->emergencyOff=emergencyOff;
};
unsigned long Valve::getEmergencyOff() const {
    return this->emergencyOff;
};

bool Valve::checkValveEmergencyState(){
    if(getState()==this->openState && (millis() - getOpenTime()) > getEmergencyOff()){
        setState(!this->openState);
        setOpenTime(0);
        return true;
    }
    return false;
}

void Valve::print(){
    Serial.println("Ventil " + name + " an PIN " + String(pin) + " - Status " + (state==openState?"offen":"geschlossen")); 
};

String Valve::printWeb(){
    return "<div><h4>" + name + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(pin) + "\" " + (isOpen() ? "checked" : "") + "><span class=\"slider\"></span></label></div>" +
            "<div class=\"left\"><h4 class=\"inline\">" + name + "Notaus Zeit</h4><span id=\"emergencyOff\" class=\"value\">" + String(emergencyOff / 60 / 1000) + "</span> min</div>";
}