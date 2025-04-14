/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "SHAH Storm Fiber 0321-7162451";
const char* password = "google700";

// MQTT Broker
const char* mqtt_server = "broker.emqx.io";  // Removed "mqtt://" prefix

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
float temp = 0;

// LED Pin
const int ledPin = 2;

void setup() {
  Serial.begin(115200);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);  // Broker IP/Domain and port
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
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

  // Control LED based on message
  if (String(topic) == "esp32/iot_temp") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    } else if (messageTemp == "off") {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, subscribe to topic
      client.subscribe("esp32/iot_temp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
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
  if (now - lastMsg > 5000) {  // Publish every 5 seconds
    lastMsg = now;
    
    char tempString[50];  // Allocate a larger buffer to hold the JSON string
    

    // Create a JSON format string using snprintf
    snprintf(tempString, sizeof(tempString), "{\"temperature\": %.2f}", temp);

    Serial.print("Temperature JSON: ");
    Serial.println(tempString);

    // Publish the JSON string to the topic
    client.publish("esp32/iot_temp", tempString);

    
    temp += 0.5;  // Simulate temperature increase
    if (temp > 40) {
      temp = 20;  // Reset temperature
    }
  }
}
