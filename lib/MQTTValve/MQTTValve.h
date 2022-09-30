#ifndef _MQTTVALVE_H
#define _MQTTVALVE_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Valve.h>
#include <Sensor.h>
#include <arduino.h>

#ifdef USEDHT
  #include <DHT.h>
  #include <DHTSensor.h>
#endif

#ifdef USEONEWIRE
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include <OneWireSensor.h>
#endif

#ifdef MOISTURESENSOR
  #include <CapacitiveMoistureSensor.h>
#endif

class MQTTValve{
    protected:
        PubSubClient client;
        const char* myHostname;
        const char* server;
        int port = 1887;
        const char* username;
        const char* password;
        unsigned long defaultPublishTime  =  10*60*1000; // Zehn Minuten
        unsigned long lastMsg = 0;
        std::function<void ()> publishState;
    public:
        MQTTValve(WiFiClient espClient);
        MQTTValve(WiFiClient espClient,
                  const char* myHostname,
                  const char* server,
                  const char* username,
                  const char* password);
        MQTTValve(WiFiClient espClient,
                  const char* myHostname,
                  const char* server,
                  int port,
                  const char* username,
                  const char* password);

        void start(std::function<void (char *, uint8_t *, unsigned int)> callback, std::function<void ()> publishState);
        void loop();
        bool connected();
        void reconnect();
        void publish(Valve* valve,int sensorID);
        void publish(Sensor* sensor,int sensorID);
        void publish(String topic, float value, bool retained);
        void publish(String topic, const char* value, bool retained);
        void publish(const char* topic, const char* value, bool retained);
        const String getTopicPublishIP();
        const String getTopicPublishSignal();
        const String getTopicPublishLastSeen();
        const String getTopicPublishTopic();
        const String getTopicSetTopic();
        const String getTopicSubscriptionTopic();
        const String getSubTopicValve();
        const String getSubTopicValve(int valveNumber);
        const String getSubTopicValveState(int valveNumber);
        const String getSubTopicValveEmergencyOff(int valveNumber);
        const String getSubTopicMoisture(int sensorNumber);
        const String getSubTopicMoistureRaw(int sensorNumber);
        const String getSubTopicHumidity(int sensorNumber);
        const String getSubTopicTemperature(int sensorNumber);

        bool shouldPublishState();
        
};

#endif