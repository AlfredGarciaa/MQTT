#include <WiFi.h>
#include <PubSubClient.h>
#define LED_BUILTIN 2

const char* WIFI_SSID = "Flia. Garcia - Arias";
const char* WIFI_PASS = "ramces2021";

const char* MQTT_BROKER_HOST = "broker.hivemq.com";
const int MQTT_BROKER_PORT = 1883;

const char* MQTT_CLIENT_ID = "GRUPO4";
const char* SUBCRIBE_TOPIC = "ucb/grupo4_in";
const char* PUBLISH_TOPIC = "ucb/grupo4";

// --------------------------- Constantes para los pines
const int PULSADOR = 33;
const int RED_LED_PIN = 14;
const int YELLOW_LED_PIN = 27;
const int GREEN_LED_PIN = 26;
const int PRENDE_LED_PIN = 25;
// --------------------------- Variable para almacenar el estado del PULSADOR
int estado;
//--------------------------- SENSOR
const int TRIGGER_PIN = 12;
const int ECHO_PIN = 13;
      
unsigned long previousMillis = 0;      
const long interval = 500;         
int ledState = LOW;
int cm = 0;

long readUltrasonicDistance(int triggerPin, int echoPin) {
    pinMode(triggerPin, OUTPUT); 
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    pinMode(echoPin, INPUT);
    return pulseIn(echoPin, HIGH);
}

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void callback(const char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += String((char) payload[i]);
  }
  // ---------------------------------------  TEMPORAL 
  if (String(topic) == SUBCRIBE_TOPIC) {
    Serial.println("Message from topic " + String(SUBCRIBE_TOPIC) + ":" + message);
    if (message == "LED_4ON") {
      digitalWrite(PRENDE_LED_PIN, HIGH);
    } else if (message == "LED_4OFF") {
      digitalWrite(PRENDE_LED_PIN, LOW);
    }
          
    if (message == "Datos") {
      cm = 0.01723 * readUltrasonicDistance(TRIGGER_PIN, ECHO_PIN);   
      mqttClient.loop();
      mqttClient.subscribe("asdasd");
      mqttClient.publish(PUBLISH_TOPIC,String(cm).c_str());
    } 
  }
}

boolean mqttClientConnect() {
  Serial.println("Connecting to " + String(MQTT_BROKER_HOST));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println("Connected to " + String(MQTT_BROKER_HOST));

    mqttClient.subscribe(SUBCRIBE_TOPIC);

    Serial.println("Subscribed to " + String(SUBCRIBE_TOPIC));
  } else {
    Serial.println("Can't connect to " + String(MQTT_BROKER_HOST));
  }
  return mqttClient.connected();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to " + String(WIFI_SSID));

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PRENDE_LED_PIN, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Can't connect to " + String(WIFI_SSID));
    while (1) delay (200);
  }
  Serial.println("Connected to " + String(WIFI_SSID));

  mqttClient.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  mqttClient.setCallback(callback);

  // PULSADOR configurado como entrada de datos
      pinMode(PULSADOR,INPUT);
  // LED configurado como salida de datos
      pinMode(PRENDE_LED_PIN,OUTPUT);
}

unsigned long previousConnectMillis = 0;
unsigned long previousPublishMillis = 0;

unsigned char counter = 0;


void cmComparar(){
    cm = 0.01723 * readUltrasonicDistance(TRIGGER_PIN, ECHO_PIN);   
    mqttClient.loop();
    mqttClient.subscribe("asdasd");

    Serial.print(cm);   
    Serial.println(" cm");
    unsigned long currentMillis = millis();
  if (cm < 10) {
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, LOW);
    } else if (cm < 20) {
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, HIGH);
       
        digitalWrite(GREEN_LED_PIN, LOW);
    } else if (cm < 30) {
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(GREEN_LED_PIN, HIGH);
     
    } else {
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }
        digitalWrite(RED_LED_PIN, ledState);
        digitalWrite(YELLOW_LED_PIN, ledState);
        digitalWrite(GREEN_LED_PIN, ledState);
      }
    }
}

void loop() {
  unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - previousConnectMillis >= 4000) {
      previousConnectMillis = now;
      if (mqttClientConnect()) {
        previousConnectMillis = 0;
      } else delay(1000);
    }
  } else {
    mqttClient.loop();
    delay(20);

    if (now - previousPublishMillis >= 8000) {
      previousPublishMillis = now;
      String message = String(counter++);
      mqttClient.publish(PUBLISH_TOPIC, message.c_str());
    }
  }
  
    cmComparar();
    
    delay(100);

        // Si el pulsador es pulsado cuando el LED está apagado...
          if(digitalRead(PULSADOR)){
            // ... se enciende el LED
                Serial.println("LED_40N");
                digitalWrite(PRENDE_LED_PIN, HIGH);
                     
            // Se establece un retraso de medio segundo entre pulsaciones
                delay(5000);
                Serial.println("LED_4OFF");
                digitalWrite(PRENDE_LED_PIN,LOW);
          } 
}
