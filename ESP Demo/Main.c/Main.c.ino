#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

//Add you WIFI Connectionn here in order to make sure the ESP can conenct to the internet

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"

// ADD your cumulocity URL here, if you don´t have one you could start one for free as a trial
// In this example the unsecure 1883 port is used. We recommend to use 8883 together with SSL.

#define MQTT_HOST "YOUR_CUMULOCITY_URL"
#define MQTT_PORT 1883

//Your can get your Tenant ID from your administrator, it starts with "t", e.g. t1231231235
// Your Tenat User start with your ID and a backslah, followd by user, e.g. t2131324124/myuser

#define TENANT_ID "YOUR_TENANT_ID"
#define TENANT_USER "YOUR_USER"
#define TENANT_PASSWORD "YOUR_PASSWORD"

//The Topic for static messages with build in (e.g. battery, signalstrength etc.). If you want to use costom templates you need to use "s/uc/TEMPLATENAME"
//The device ID you use for the connection will be used as identifier. If the device was not created before it will be created, but with prefix "My MQTT Device"

#define TOPIC "s/us"
#define DEVICE_ID "YOUR_DEVICE_ID"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void sendWiFiStrenght(long value){
String payload = "210," + String(value);
mqttClient.publish(TOPIC,0,true,payload.c_str());
}

//Creating a Critical Alarm via Payload 301 and the given Alarmtype on the build in static topic

void createCriticalAlarm(String Alarmtype){
String payload = "301," + Alarmtype;
mqttClient.publish(TOPIC,0,true,payload.c_str());
}

//Creating a Warning via Payload 304 and the given Alarmtype on the build in static topic

void createWarninglAlarm(String Alarmtype){
String payload = "304," + Alarmtype;
mqttClient.publish(TOPIC,0,true,payload.c_str());
}

//Events are created via 400. In this example the type of the Event is just c8y

void createEvent(){
mqttClient.publish(TOPIC,0,true,"400,c8y");
}

void setup() {
  //Starting Serialconnector
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  //MQTT
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setClientId(DEVICE_ID);
  mqttClient.setCredentials(TENANT_USER,TENANT_PASSWORD);

  //Connect to Wifi
  connectToWifi();
}

void loop() {
  sendWiFiStrenght(WiFi.RSSI());
  delay(2000);
}
