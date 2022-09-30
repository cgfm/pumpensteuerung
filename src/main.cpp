#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <LittleFS.h>

#define USEDHT
#ifdef USEDHT
  #include <DHTSensor.h>
#endif

#define USEONEWIRE
#ifdef USEONEWIRE
  #include <OneWire.h>
#endif

#define USEMOISTURESENSOR
#ifdef USEMOISTURESENSOR
  #include <CapacitiveMoistureSensor.h>
#endif

#include <time.h>
#include <Valve.h>
#include <MQTTValve.h>


// Webserver
AsyncWebServer server(80);

// Ventilsteuerung
#define EMERGENCYOFF (unsigned long)(60 * 60 * 1000)
bool closedState = HIGH;
bool openState = LOW;

#define VALVECOUNT 3
Valve* myValves[VALVECOUNT+1] = {
    new Valve("leer", D1, closedState, EMERGENCYOFF),
    new Valve("Büsche Weg", D2, closedState, EMERGENCYOFF),
    new Valve("Hochbeete", D3, closedState, EMERGENCYOFF),
    NULL};


#define SENSORCOUNT 3
#if !defined(USEDHT) && !defined(USEONEWIRE) && !defined(USEMOISTURESENSOR)
  Sensor* mySensors[1] = {NULL};
#else
Sensor* mySensors[SENSORCOUNT+1] = {
    new CapacitiveMoistureSensor(A0, "Bodenfeuchtesensor", 870, 315),
    new DHTSensor(D6, "DHT Garage", D5, DHT22),
    new OneWireSensor(D0, "Außentemperatur"),
    NULL};
#endif

// WiFi
const char *myHostname = "Testventilbox";
const char *ssid = "fritzWLAN";
const char *password = "bxanbBqN07SYSeEFY3Qw";
WiFiClient espClient;

// MQTT
const char *mqtt_username = "MaschinennetzBroker";
const char *mqtt_password = "DW7HpU.g8.kdXAZB7sJMAxs_B8OAT(";

MQTTValve mqttValve = MQTTValve(espClient,
                                myHostname,
                                "192.168.178.200",
                                mqtt_username,
                                mqtt_password);

#define MY_NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"
time_t now; // this is the epoch
tm tmInfo;  // the structure tm holds time information in a more convient way


#include "websitePages.cpp"

// Setting up WiFi connection
void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);

  WiFi.hostname(myHostname);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  randomSeed(micros());

  Serial.println("");
  Serial.print("WiFi connected (IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println(")");
}

String getFormattedTime(){
    time(&now);                       // read the current time
  localtime_r(&now, &tmInfo);           // update the structure tmInfo with the current time
  String currTime = 
                String(tmInfo.tm_year + 1900) + "-" + 
                (tmInfo.tm_mon + 1<10?"0":"") + String(tmInfo.tm_mon + 1) + "-" + 
                (tmInfo.tm_mday<10?"0":"") + tmInfo.tm_mday + " " + 
                (tmInfo.tm_hour<10?"0":"") + tmInfo.tm_hour + ":" + 
                (tmInfo.tm_min<10?"0":"") + tmInfo.tm_min + ":" + 
                (tmInfo.tm_sec<10?"0":"") + tmInfo.tm_sec;
                
  return "[" + currTime + "] ";
}

void deleteLines(String filename){
  File original = LittleFS.open(filename, "r");
  if (!original){
    Serial.print("- failed to open file "); Serial.println(filename);
  }else{
    //We skip the first 50 lines
    for(int i=0;i<50;i++){
      original.readStringUntil('\n');
    }
    File temporary = LittleFS.open(filename+".new", "w+");
    if(!temporary){
      Serial.println("-- failed to open temporary file "); 
    }else{
      while(original.available()){
        temporary.print(original.readStringUntil('\n')+"\n");
      }
      temporary.close(); 
    }
    original.close();    

    LittleFS.remove(filename);
    LittleFS.rename(filename+".new",filename);
  }
}

//File write line to log
bool writeLog(const char* logline){
  File logFile = LittleFS.open("/log.txt", "a+"); //open the log file
  if (!logFile){
    Serial.println("Failed to open log file");
    return false;
  }else{
    logFile.println(logline);

    if(logFile.size()>10240){
      logFile.close();  //Close file
      Serial.println("File Closed");
      deleteLines("/log.txt");
    }else{
      logFile.close();
    }
  }
  return true;
}

String getLogFileContent(){
  String out="";
  File logFile = LittleFS.open("/log.txt", "r"); //open the log file
  if (!logFile){
    return "Failed to open log file";
  }else{
      while(logFile.available()){
        out +=logFile.readStringUntil('\n')+"<br>\n";
      }
  }
  logFile.close();
  return out;
}

void formatFilesystem(){
  bool formatted = LittleFS.format();
 
  if(formatted){
    Serial.println("formatFilesystem(): Success formatting");
  }else{
    Serial.println("formatFilesystem(): Error formatting");
  }
}

void clearLogFileContent(){
  File logFile = LittleFS.open("/log.txt", "w"); //open the log file
  logFile.close();
}

String getLogFileSize(){
  String out="";
  File logFile = LittleFS.open("/log.txt", "r"); //open the log file
  if (!logFile){
    return "Failed to open log file";
  }else{
    if(logFile.size()>1024){
        out +=String(logFile.size()/1024)+" kB";
    }else{
        out +=String(logFile.size())+" B";
    }
  }
  logFile.close();
  return out;
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String value = "";
  for (unsigned int i = 0; i < length; i++)
  {
    value += (char)payload[i];
  }

  String topicShort = topic;
  topicShort.replace(mqttValve.getTopicSetTopic(), "");
  if(topicShort.startsWith(mqttValve.getSubTopicValve())){
    //Valve specific topics
    topicShort.replace(mqttValve.getSubTopicValve(), "");
    int valveID = topicShort.substring(0, topicShort.indexOf("/")-1).toInt();

    Serial.print("VentilID: ");
    Serial.println(valveID);
    
    Valve* valve = Valve::getValveFromID(myValves, valveID);

    if (topicShort.equals(mqttValve.getSubTopicValveEmergencyOff(valveID))){
      valve->setEmergencyoff(value.toInt() * 60 * 1000);
      mqttValve.publish(mqttValve.getTopicPublishTopic() + mqttValve.getSubTopicValveEmergencyOff(valveID), value.c_str(), true);
      writeLog((getFormattedTime() + "[info] SetState - " + valve->getName() + " emergencyOff: " + value + " min").c_str());
    }
    if (topicShort.equals(mqttValve.getSubTopicValveState(valveID))){
      valve->setState(value.c_str());
      mqttValve.publish(mqttValve.getTopicPublishTopic() + mqttValve.getSubTopicValveState(valveID), value.c_str(), true);
      writeLog((getFormattedTime() + "[info] SetState " + valve->getName() + ", payload: " + value).c_str());
    }
  }
}

void publishState()
{
  String out = "";
  time_t currTime;
  time(&currTime);

  mqttValve.publish(mqttValve.getTopicPublishIP(), WiFi.localIP().toString().c_str(), true);
  mqttValve.publish(mqttValve.getTopicPublishSignal(), String(WiFi.RSSI()).c_str(), true);
  mqttValve.publish(mqttValve.getTopicPublishLastSeen(), ctime(&currTime), true);

  int i = 0;
  while(myValves[i]!=NULL){
    mqttValve.publish(myValves[i],i+1);
    i++;
  }
  
  i = 0;
  while(mySensors[i]!=NULL){
    mqttValve.publish(mySensors[i], i+1);
    i++;
  }
}

String processor(const String &var)
{
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    int i = 0;
    while(myValves[i]!=NULL){
      buttons += myValves[i]->printWeb();
      i++;
    }

    i = 0;
    while(mySensors[i]!=NULL){
        buttons += mySensors[i]->printWeb();
        i++;
    }

    buttons += "<div class=\"left\"><h4 class=\"inline\">Log</h4><span id=\"logsize\" class=\"value\">" + getLogFileSize() + "</span></label></div>";
    buttons += "<div class=\"left\"><h4 class=\"inline\">WiFi Signal</h4><span id=\"logsize\" class=\"value\">" + String(WiFi.RSSI()) + "</span> dBm</label></div>";
    return buttons;
  }

  if (var == "LOGFILEPLACEHOLDER")
  {
    return getLogFileContent();
  }
  if (var == "HOSTNAMEPLACEHOLDER")
  {
    return myHostname;
  }
  return String();
}

void initWebserver()
{
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, processor); });
  server.on("/publishState", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    publishState();
    request->send_P(200, "text/html", index_html, processor); });

  server.on("/setState", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (request->hasParam("ventil") && request->hasParam("state")) {
      String valve = request->getParam("ventil")->value();
      Valve::getValveFromID(myValves,valve.toInt())->setState(
          request->getParam("state")->value().c_str()
      );
      request->send(200, "text/plain", request->getParam("state")->value());
    }else{
      request->send(200, "text/plain", "unknown");
    } });
  server.on("/clearLog", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    clearLogFileContent();
    request->send_P(200, "text/html", index_html, processor); });
  server.on("/getLog", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send_P(200, "text/html", log_html, processor); });

  server.begin();
}

void setup()
{
  Serial.begin(115200);

  setup_wifi();
  mqttValve.start(mqttCallback, publishState);

  // mount the file system
  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount file system");
  }

  configTime(MY_TZ, MY_NTP_SERVER);

  AsyncElegantOTA.begin(&server, mqtt_username, mqtt_password); // Start ElegantOTA
  initWebserver();
}

void loop()
{
  mqttValve.loop();

  if (mqttValve.shouldPublishState())
  {
    publishState();
  }

  int i = 0;
  while(myValves[i]!=NULL){
    if (myValves[i]!=nullptr && myValves[i]->checkValveEmergencyState())
    {
      myValves[i]->setState(!myValves[i]->getOpenState());
      mqttValve.publish(myValves[i], i);
      writeLog((getFormattedTime() + "[error] Ventil:" + myValves[i]->getName() + ", payload: OFF - EMERGENCY").c_str());
    }
    i++;
  }
}