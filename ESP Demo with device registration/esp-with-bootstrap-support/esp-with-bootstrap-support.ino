
// Code has been tested on Wemos D1 Mini (ESP8266) and multipe TTGO boards (ESP32)
// ESP8266 will store the credentials received within the bootstrap process within the SPI, if available otherwise you will have to perform the bootstrapping on each reboot
// ESP32 will store the credentials received within the bootstrap process within the internal flash memory

#include "config.h"
const int ledPin = LEDPIN;

#ifdef ARDUINO_ARCH_ESP32
// ESP32
  #include <Preferences.h>
  Preferences preferences; 
  #ifdef USE_SSL
    #include <WiFiClientSecure.h>
  #else 
    #include <WiFi.h>
  #endif
#else 
// ESP8266
  #include <ESP8266WiFi.h>
  #include <FS.h>
  #include <ArduinoJson.h>
#endif

#if HAS_SENSORS
  #include <Adafruit_BME280.h>
  #include <Adafruit_Sensor.h>
#endif

#include <PubSubClient.h>
#include <Wire.h>

#define CONFIG_DIR "az"

boolean bootstapping = true;
#if USE_SSL
  WiFiClientSecure espClient;
#else
  WiFiClient espClient;
#endif

PubSubClient client(espClient);
long lastMsg = 0;
String deviceId = "";
boolean ledOn = false;

#if HAS_SENSORS
Adafruit_BME280 bme; // I2C
#endif

float temperature = 0;
float humidity = 0;

void loadCredentials() {
  #ifdef ARDUINO_ARCH_ESP32
    preferences.begin(CONFIG_DIR, false);
  
    //preferences.clear();
  
    mqttUser = preferences.getString("user", BOOTSTRAP_USER);
    mqttPassword = preferences.getString("password", BOOTSTRAP_PASSWORD);
    
    Serial.println("Using Credentials:");
    Serial.println(mqttUser);
    Serial.println(mqttPassword);
  
    preferences.end();
  #else
    //SPIFFS.format();
    if (SPIFFS.begin()) {
      Serial.println("mounted file system");
      if (SPIFFS.exists(CONFIG_DIR)) {
        //file exists, reading and loading
        Serial.println("reading config file");
        File configFile = SPIFFS.open(CONFIG_DIR, "r");
        if (configFile) {
          Serial.println("opened config file");
          size_t size = configFile.size();
          if (size > 1024) {
            Serial.println("Config file size is too large");
            return;
          }
  
          // Allocate the memory pool on the stack.
          // Use arduinojson.org/assistant to compute the capacity.
          StaticJsonDocument<256> jsonBuffer;
        
          // Parse the root object
          DeserializationError error = deserializeJson(jsonBuffer, configFile);
  
          // Test if parsing succeeds.
          if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            Serial.println(F("Failed to read file, Starting Bootstrap process"));
            return;
          } else if (jsonBuffer.containsKey("user") && jsonBuffer.containsKey("password")) {
            // Copy values from the JsonObject to the Config
            mqttUser = jsonBuffer["user"].as<String>();
            mqttPassword = jsonBuffer["password"].as<String>();
          } else {
            Serial.println(F("Failed to read file, Starting Bootstrap process"));
          }
          configFile.close();
        }
      }
    } else {
      Serial.println("failed to mount FS");
    }
  #endif
  Serial.print("Using user: ");
  Serial.println(mqttUser);
  if (mqttUser != BOOTSTRAP_USER) {
    bootstapping = false;
    Serial.println("Not in Bootstrap mode");
  }
}

void storeCredentials(String user, String password) {
  #ifdef ARDUINO_ARCH_ESP32
    preferences.begin(CONFIG_DIR, false);
  
    preferences.clear();
    preferences.putString("user", mqttUser);
    preferences.putString("password", mqttPassword);
    preferences.end();
  #else
    if (SPIFFS.begin()) {
      File file = SPIFFS.open(CONFIG_DIR, "w");
      if(!file){
       Serial.println("There was an error opening the file for writing");
       return;
      } else {
        StaticJsonDocument<256> doc;
        
        doc["user"] = user;
        doc["password"] = password;
        serializeJson(doc, Serial);
        Serial.println("");
        if(serializeJson(doc, file)) {//file.print(payload)
          Serial.println("File was written");
        } else {
          Serial.println("File write failed");
        }
        file.close();
      }
    }
  #endif
}

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void setup() {
  Serial.begin(115200);
  Serial.println("");

  loadCredentials();

  String mac = WiFi.macAddress();
  mac.replace(":", "");
  deviceId = "ESP-" + mac;
  Serial.println("DeviceId:");
  Serial.println(deviceId);
  #if HAS_SENSORS 
    if (!bme.begin(0x76)) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
    }
  #endif
  setup_wifi();
  #if USE_SSL
    #ifdef ARDUINO_ARCH_ESP32
      espClient.setCACert(root_ca);
    #else
      espClient.setCACert_P(caCert,caCertLen);
      setClock();
    #endif
  #endif
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
      if (data.charAt(i) == separator || i == maxIndex) {
          found++;
          strIndex[0] = strIndex[1] + 1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
      }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == BOOTSTRAP_SUBSCRIBE_TOPIC) {
    Serial.println("Received Bootstrap credentials:");
    Serial.println(messageTemp);
    String messagetype = getValue(messageTemp, ',', 0);
    if (messagetype == "70") {
      String newTenant = getValue(messageTemp, ',', 1);
      String newUser = getValue(messageTemp, ',', 2);
      String newPassword = getValue(messageTemp, ',', 3);
      mqttUser = newTenant + '/' + newUser;
      mqttPassword = newPassword;

      storeCredentials(mqttUser, mqttPassword);
  
      Serial.println("New Credentials:");
      Serial.println(mqttUser);
      Serial.println(mqttPassword);
      bootstapping = false;
      client.disconnect();
    } else {
      Serial.println("Error on Bootstrap Topic");
    }
  } else if (String(topic) == OPERATIONS_TOPIC) {
    Serial.println("Received Operation:");
    Serial.println(messageTemp);
    String messagetype = getValue(messageTemp, ',', 0);
    if (messagetype == "510") {
      Serial.println("Simulating restart:");
      Serial.println("Set operation to executing..");
      client.publish(SEND_TOPIC,"501,c8y_Restart");
      Serial.println("Sleep 5 seconds");
      delay(5000);
      Serial.println("Set operation to success..");
      client.publish(SEND_TOPIC,"503,c8y_Restart");
    } else if (messagetype == "511") {
      String devId = getValue(messageTemp, ',', 1);
      String command = getValue(messageTemp, ',', 2);
      if (command == "LEDON"){
        ledOn = true;
        digitalWrite(ledPin, LEDON);
        Serial.println(command);
        client.publish(SEND_TOPIC,"501,c8y_Command");
        client.publish(SEND_TOPIC,"503,c8y_Command");
      } else if (command == "LEDOFF"){
        ledOn = false;
        digitalWrite(ledPin, LEDOFF);
        Serial.println(command);
        client.publish(SEND_TOPIC,"501,c8y_Command");
        client.publish(SEND_TOPIC,"503,c8y_Command");
      } else {
        Serial.println("Simulating execution:");
        Serial.println("Set operation to executing..");
        client.publish(SEND_TOPIC,"501,c8y_Command");
        Serial.println("Sleep 5 seconds");
        digitalWrite(ledPin, LEDON);
        delay(5000);
        if (!ledOn) {
          digitalWrite(ledPin, LEDOFF);
        }
        Serial.println("Set operation to success..");
        client.publish(SEND_TOPIC,"503,c8y_Command");
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting ");
    if (USE_SSL) {
      Serial.print("MQTTS");
    } else {
      Serial.print("MQTT");
    }
    Serial.print(" connection to: ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.print(MQTT_PORT);
    Serial.println("...");
    // Attempt to connect
    if (client.connect(deviceId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {
      Serial.println("connected");
      // Subscribe
      if (bootstapping) {
        client.subscribe(BOOTSTRAP_SUBSCRIBE_TOPIC);
      } else {
        // subscribe to channels after bootstrapping here:
        client.subscribe(OPERATIONS_TOPIC);
        String devName = "100," + String(DEVICE_NAME);
        client.publish(SEND_TOPIC, devName.c_str());
        client.publish(SEND_TOPIC, "114,c8y_Command,c8y_Configuration,c8y_Restart");
        client.publish(SEND_TOPIC, "117,2");
        String mac = WiFi.macAddress();
        mac.replace(":", "");
        String hwDetails = "110," + mac + "," + HW_MODEL + "," + HW_REVISION;
        Serial.println(hwDetails);
        client.publish(SEND_TOPIC,hwDetails.c_str());
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis(); 
  if (bootstapping) {
    if (now - lastMsg > 5000) {
      digitalWrite(ledPin, LEDON);
      lastMsg = now;
      Serial.println("Polling Credentials: ");
      client.publish(BOOTSTRAP_POLL_TOPIC, "");
      digitalWrite(ledPin, LEDOFF);
    }
  } else {
    if (now - lastMsg > 30000) {
      digitalWrite(ledPin, LEDON);
      lastMsg = now;
      String operationsPoll = "500";
      Serial.print("Polling Operations: ");
      Serial.println(operationsPoll);
      client.publish(SEND_TOPIC, operationsPoll.c_str());
      
      String rssi = "210," + String(WiFi.RSSI());
      Serial.print("Sending RSSI: ");
      Serial.println(rssi);
      client.publish(SEND_TOPIC, rssi.c_str());
      #if HAS_SENSORS
        // temperature
        temperature = bme.readTemperature();
        // Convert the value to a char array
        char tempString[8];
        dtostrf(temperature, 1, 2, tempString);
        String temperatureMessage = "211," + String(tempString);
        Serial.print("Sending Temperature: ");
        Serial.println(temperatureMessage);
        client.publish(SEND_TOPIC, temperatureMessage.c_str());

        // humidity
        humidity = bme.readHumidity();
        // Convert the value to a char array
        char humString[8];
        dtostrf(humidity, 1, 2, humString);
        String humidityMessage = "200,c8y_Humidity,H," + String(humString) + ",%";
        Serial.print("Sending Humidity: ");
        Serial.println(humidityMessage);
        client.publish(SEND_TOPIC, humidityMessage.c_str());
      #endif
      if (!ledOn) {
        digitalWrite(ledPin, LEDOFF);
      }
    }
  }
}
