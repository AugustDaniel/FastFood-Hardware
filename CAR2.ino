// Motor A connections
const int enA = 32;
const int in1 = 22;
const int in2 = 18;

// Motor B connections
const int enB = 33;
const int in3 = 19;
const int in4 = 21;

// Light locks connections
const int pin12 = 34;
const int pin34 = 35;
int pwm12 = 255;
int pwm34 = 255;
int gates12 = 0;
int gates34 = 0;
int counter = 0;
bool high12 = true;
bool high34 = true;
const int prefGates = 31;

//light sensor connections
const int pinLight = 39;
bool passedRed = false;
const int checkPoint = 250;
const int finishLine = 4000;


// mqtt drive connection
bool left = false;
const String topicLeft = "avanstibreda/ti/1.4/A1/Test/LEFT";
bool right = false;
const String topicRight = "avanstibreda/ti/1.4/A1/Test/RIGHT";
bool forward = false;
const String topicForward = "avanstibreda/ti/1.4/A1/Test/GAS";
bool backwards = false;
const String topicBackwards = "avanstibreda/ti/1.4/A1/Test/BREAK";
bool isStopped = true;


#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Galaxy S20 5G66bc"; // Enter your WiFi name
const char *password = "wtke8365";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "avanstibreda/ti/1.4/A1/Test/#";
const char *topicLine = "avanstibreda/ti/1.4/A1/Test/LINE";
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
    client.publish(topicLine, "connected");
    client.subscribe(topic);
  
	// Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// // Turn off motors - Initial state
  analogWrite(enA, pwm12);
	analogWrite(enB, pwm34);
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

  // Set all sensors control pins to inputs
  pinMode(pin12, INPUT);
  pinMode(pin34, INPUT);
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

    if(Topic == topicRight) {
      if ((char)payload[0] == 't') {
        right = true;
      } else if ((char)payload[0] == 'f') {
        right = false;
      }
    } else if (Topic == topicLeft) {
      if ((char)payload[0] == 't') {
        left = true;
      } else if ((char)payload[0] == 'f') {
        left = false;
      }
    } else if (Topic == topicForward) {
      if ((char)payload[0] == 't') {
        forward = true;
      } else if ((char)payload[0] == 'f') {
        forward = false;
      }
    } else if (Topic == topicBackwards) {
      if ((char)payload[0] == 't') {
        backwards = true;
      } else if ((char)payload[0] == 'f') {
        backwards = false;
      }
    }

    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println("");
    setServos();
}

void setServos() {
  if (left) {
    driveLeft();
  } else if (right) {
    driveRight();
  } else if (forward) {
    driveForward();
  } else if (backwards) {
    driveBackwards();
  } else {
    stop();
  }
}

void getValues() {
  int value12 = analogRead(pin12);
  int value34 = analogRead(pin34);
  // Serial.println(value12);
  // Serial.println(value34);

  if (value12 > 4000 && !high12) {
    high12 = true;

  } else if (value12 < 4000 && high12) {
    high12 = false;
    gates12++;
  }
  if (value34 > 4000 && !high34) {
    high34 = true;

  } else if (value34 < 4000 && high34) {
    high34 = false;
    gates34++;
  }
  counter++;
  if (counter == 500) {
    setPwm();
  }

  int valueLight = analogRead(pinLight);
  // Serial.println(valueLight);

  if(valueLight > finishLine & passedRed){
    client.publish(topicLine, "z");
    passedRed = false;
  }

  if(valueLight < checkPoint & !passedRed){
    client.publish(topicLine, "r");
    passedRed = true;
  }
}

void setPwm() {
  // Serial.println(gates12);
  // Serial.println(pwm12);
  // Serial.println(gates34);
  // Serial.println(pwm34);
  counter = 0;
    if (isStopped) {
      return;
    }
    if (gates12 < prefGates && pwm12 < 255 && !right) {
      pwm12 += 2.5;
    } else if (gates12 > prefGates && pwm12 > 0 && !right) {
      pwm12 -= 2.5;
    }
    if (gates34 < prefGates && pwm34 < 255 && !left) {
      pwm34 += 2.5;
    } else if (gates34 > prefGates && pwm34 > 0 && !left) {
      pwm34 -= 2.5;
    }
    setSpeed();
    gates12 = 0;
    gates34 = 0;
}

void setSpeed() {
    analogWrite(enA, pwm12);
	  analogWrite(enB, pwm34);
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