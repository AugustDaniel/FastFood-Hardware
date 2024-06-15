// Motor A connections
const int enA = 32;
const int in1 = 22;
const int in2 = 18;

// Motor B connections
const int enB = 33;
const int in3 = 19;
const int in4 = 21;

//light sensor connections
const int pinLight = 39;
bool passedRed = false;
const int checkPoint = 250;
const int finishLine = 4000;


// mqtt drive connection
const String topicLeft = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/LEFT";
bool left = false;
const String topicRight = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/RIGHT";
bool right = false;
const String topicForward = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/GAS";
bool forward = false;
const String topicBackwards = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/BREAK";
bool backwards = false;
char lastSignal = 's';
bool isStopped = true;


#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Galaxy S20 5G66bc"; // Enter your WiFi name
const char *password = "wtke8365";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/#";
const char *topicLine = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/LINE";
const char *topicIsClaimed = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/isClaimed";
const char *topicReset = "avanstibreda/ti/1.4/A1/Pepperoni-Racer/RESET";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {
// // Set software serial baud to 115200;
Serial.begin(115200);
// Connecting to a Wi-Fi network
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
}

client.setServer(mqtt_broker, mqtt_port);
client.setCallback(callback);
while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
        Serial.println("Public EMQX MQTT broker connected");
    } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
    }

}

// Publish and subscribe  
    client.publish(topicIsClaimed, "f");
    client.subscribe(topic);
  
	// Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// // Turn off motors - Initial state
  analogWrite(enA, 255);
	analogWrite(enB, 255);
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

  // Set all sensors control pins to inputs
  Serial.println("start new");
}

void loop() {
  client.loop();
  getValues();
  delay(2);
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    String Topic = (String) topic;
    isStopped = false;

    if (Topic == topicReset) {
      client.publish(topicIsClaimed, "f");
      left = false;
      right = false;
      forward = false;
      backwards = false;
    }

    if(Topic == topicRight) {
      if ((char)payload[0] == 't') {
        right = true;
        lastSignal = 'r';
      } else if ((char)payload[0] == 'f') {
        right = false;
        lastSignal = 's';
      }
    } else if (Topic == topicLeft) {
      if ((char)payload[0] == 't') {
        left = true;
        lastSignal = 'l';
      } else if ((char)payload[0] == 'f') {
        left = false;
        lastSignal = 's';
      }
    } else if (Topic == topicForward) {
      if ((char)payload[0] == 't') {
        forward = true;
        lastSignal = 'f';
      } else if ((char)payload[0] == 'f') {
        forward = false;
        lastSignal = 's';
      }
    } else if (Topic == topicBackwards) {
      if ((char)payload[0] == 't') {
        backwards = true;
        lastSignal = 'b';
      } else if ((char)payload[0] == 'f') {
        backwards = false;
        lastSignal = 's';
      }
    }

    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println("");
    setServos();
}

void setServos() {
  Serial.println(lastSignal);
  if (lastSignal == 's') {
    if (left) {
      lastSignal = 'l';
    } else if (right) {
      lastSignal = 'r';
    } else if (forward) {
      lastSignal = 'f';
    } else if (backwards) {
      lastSignal = 'b';
    } else {
      stop();
      return;
    }
  }
  
  if (lastSignal == 'l') {
    driveLeft();
  } else if (lastSignal == 'r') {
    driveRight();
  } else if (lastSignal == 'f') {
    driveForward();
  } else if (lastSignal == 'b') {
    driveBackwards();
  }
}

void getValues() {
  int valueLight = analogRead(pinLight);
  Serial.println(valueLight);

  if(valueLight > finishLine & passedRed){
    client.publish(topicLine, "z");
    passedRed = false;
  }

  if(valueLight < checkPoint & !passedRed){
    client.publish(topicLine, "r");
    passedRed = true;
  }
}

void driveForward(){
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
  // Serial.print("forward");
}

void driveLeft(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
  // Serial.print("left");
}

void driveRight(){
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
  // Serial.print("right");
}

void driveBackwards(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
  // Serial.print("backwards");
}

void stop(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
  isStopped = true;
  // Serial.print("stop");
}