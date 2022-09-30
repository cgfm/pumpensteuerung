#include <MQTTValve.h>

template<typename Base, typename T>
inline bool instanceof(const T *ptr) {
   return dynamic_cast<const Base*>(ptr) != nullptr;
}

MQTTValve::MQTTValve(WiFiClient espClient){
    this->client = PubSubClient(espClient);
};
MQTTValve::MQTTValve(WiFiClient espClient,
                  const char* myHostname,
                  const char* server,
                  const char* username,
                  const char* password){
    this->client = PubSubClient(espClient);
    this->myHostname = myHostname;
    this->server=server;
    this->username=username;
    this->password=password;
};
MQTTValve::MQTTValve(WiFiClient espClient,
                  const char* myHostname,
                  const char* server,
                  int port,
                  const char* username,
                  const char* password){
    this->client = PubSubClient(espClient);
    this->myHostname = myHostname;
    this->server=server;
    this->port=port;
    this->username=username;
    this->password=password;
};

void MQTTValve::start(std::function<void (char *, uint8_t *, unsigned int)> callback, std::function<void ()> publishState){
    client.setServer(server, port);
    client.setCallback(callback);
    this->publishState=publishState;
    reconnect();
}

void MQTTValve::loop(){ 
  if (!connected()) {
    reconnect();
  }
  client.loop();
}

bool MQTTValve::connected(){
    return client.connected();
};

void MQTTValve::reconnect(){
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = String(this->myHostname) + "-" + String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),this->username,this->password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      publishState();
      Serial.println("published");
      // ... and resubscribe
      client.subscribe(getTopicSubscriptionTopic().c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MQTTValve::publish(Valve* valve, int sensorID){
      client.publish(
          (getTopicPublishTopic() + getSubTopicValveState(sensorID)).c_str(),
          (valve->isOpen() ? "ON" : "OFF"),
          true);
      client.publish(
          (getTopicPublishTopic() + getSubTopicValveEmergencyOff(sensorID)).c_str(),
          (const char*) valve->getEmergencyOff(),
          true);
}

void MQTTValve::publish(Sensor* sensor,int sensorID){
    #ifdef MOISTURESENSOR
    if (instanceof<CapacitiveMoistureSensor>(sensor)){
        CapacitiveMoistureSensor* moistureSensor = dynamic_cast<CapacitiveMoistureSensor*>(sensor);
        publish(
            getTopicPublishTopic() + getSubTopicMoisture(sensorID),
            moistureSensor->readSensor(),
            true);
        publish(
            getTopicPublishTopic() + getSubTopicMoistureRaw(sensorID),
            moistureSensor->readSensorRaw(),
            true);
    }
    #endif
    #ifdef USEDHT
    if (instanceof<DHTSensor>(sensor)){
        DHTSensor* dhtSensor = dynamic_cast<DHTSensor*>(sensor);
        dhtSensor->readSensor();
        publish(
            getTopicPublishTopic() + getSubTopicTemperature(sensorID),
            dhtSensor->getTemperature(),
            true);
        publish(
            getTopicPublishTopic() + getSubTopicHumidity(sensorID),
            dhtSensor->getHumidity(),
            true);
    }
    #endif
    #ifdef USEONEWIRE
    if (instanceof<OneWireSensor>(sensor)){
        OneWireSensor* oneWireSensor = dynamic_cast<OneWireSensor*>(sensor);
        publish(
            getTopicPublishTopic() + getSubTopicTemperature(sensorID),
            oneWireSensor->readSensor(),
            true);
    }
    #endif
};
void MQTTValve::publish(String topic, const char* value, bool retained){
    client.publish(String(topic).c_str(), value, retained);
};
void MQTTValve::publish(String topic, const float value, bool retained){
    client.publish(String(topic).c_str(), String(value,2).c_str(), retained);
};
void MQTTValve::publish(const char* topic, const char* value, bool retained){
    client.publish(topic, value, retained);
};

const String MQTTValve::getTopicPublishIP() {
    return "hosts/" + String(myHostname) +"/ip";
};
const String MQTTValve::getTopicPublishSignal() {
    return "hosts/" + String(myHostname) +"/signal";
};
const String MQTTValve::getTopicPublishLastSeen() {
    return "hosts/" + String(myHostname) +"/lastSeen";
};
const String MQTTValve::getTopicPublishTopic() {
    return "bewasserung/" + String(myHostname) +"/";
};
const String MQTTValve::getTopicSetTopic() {
    return getTopicPublishTopic() + "set/";
};
const String MQTTValve::getTopicSubscriptionTopic() {
    return getTopicSetTopic() + "#";
};
const String MQTTValve::getSubTopicValve() {
    return "ventil";
};
const String MQTTValve::getSubTopicValve(int valveNumber) {
    return getSubTopicValve() + String(valveNumber) + "/";
};
const String MQTTValve::getSubTopicValveState(int valveNumber) {
    return getSubTopicValveState(valveNumber) + "state";
};
const String MQTTValve::getSubTopicValveEmergencyOff(int valveNumber) {
    return getSubTopicValveState(valveNumber) + "emergencyOff";
};
const String MQTTValve::getSubTopicMoisture(int sensorNumber) {
    return "moisture" + sensorNumber;
};
const String MQTTValve::getSubTopicMoistureRaw(int sensorNumber) {
    return "moisture" + String(sensorNumber) + "Raw";
};
const String MQTTValve::getSubTopicHumidity(int sensorNumber) {
    return "humidity" + sensorNumber;
};
const String MQTTValve::getSubTopicTemperature(int sensorNumber) {
    return "temperature" + sensorNumber;
};

bool MQTTValve::shouldPublishState(){
    return millis() - lastMsg > defaultPublishTime;
};