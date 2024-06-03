// Motor A connections
const int enA = 32;
const int in1 = 22;
const int in2 = 18;
// Motor B connections
const int enB = 33;
const int in3 = 19;
const int in4 = 21;

int ls1;
bool passedRed =true;

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "A54 Joshua Hotspot"; // Enter your WiFi name
const char *password = "wachtwoord9";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "avanstibreda/ti/1.4/A1/auto1";
// const char *topic2 = "avanstibreda/ti/1.4/A1/auto2";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {

// Set software serial baud to 115200;
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
    client.publish(topic, "Hi, I'm ESP32 ^^");
    client.subscribe(topic);

  
	// Set all the motor control pins to outputs
	pinMode(enA, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// // Turn off motors - Initial state
  analogWrite(enA, 0);
	analogWrite(enB, 0);
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}



void loop() {
  // client.loop();
// testpwm();

ls1 = analogRead(34); 


  delay(500);
  if(ls1>3500 & passedRed==true ){
    client.publish(topic, "Een zwarte lijn!");
    passedRed=false;
  }

  if(ls1<250){
    client.publish(topic, "Een rode lijn");
    passedRed=true;
  }
// stop();
// delay(500);
  
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.println("Message:");
    // for (int i = 0; i < length; i++) {
    //     Serial.print((char) payload[i]);
    // }
    // Serial.println();
    // Serial.println("-----------------------");
    if((char)payload[0] == 'w'){
    forward();
    Serial.print("vooruit");
  } else if ((char)payload[0] == 's'){
    backwards();
        Serial.print("achteruit");

  } else if ((char)payload[0] == 'a'){
    left();
        Serial.print("links");

  } else if ((char)payload[0] == 'd'){
    right();
        Serial.print("rechts");

  }  else if ((char)payload[0] == 'q'){
    stop();
        Serial.print("stop");

  } else{
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
  }
}

void testpwm(){
fast();
  delay(1000);
  midlefast();
  delay(1000);
  midle();
  delay(1000);
  midleslow();
  delay(1000);
  slow();
  delay(1000);
}


void fast(){
  analogWrite(enA, 254);
	analogWrite(enB, 254);
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
  Serial.print("fast");
}

void midlefast(){
  analogWrite(enA, 200);
	analogWrite(enB, 200);
  Serial.print("midlefast");
}

void midle(){
  analogWrite(enA, 155);
	analogWrite(enB, 155);
  Serial.print("midle");
}

void midleslow(){
  analogWrite(enA, 100);
	analogWrite(enB, 100);
  Serial.print("midleslow");
}

void slow(){
  analogWrite(enA, 0);
	analogWrite(enB, 0);
  Serial.print("slow");
}

void forward(){
  // analogWrite(enA, 255);
	// analogWrite(enB, 155);
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
  
  Serial.print("forward");
  
}

void left(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
  Serial.print("left");
}

void right(){
  digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
  Serial.print("right");
}

void backwards(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
  Serial.print("backwards");
}

void stop(){
  digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
  Serial.print("stop");
}


// This function lets you control spinning direction of motors
void directionControl() {
  // client.loop();
	// Set motors to maximum speed
	// For PWM maximum possible values are 0 to 255
	analogWrite(enA, 255);
	analogWrite(enB, 255);

	// Turn on motor A & B
	digitalWrite(in1, HIGH);
	digitalWrite(in2, LOW);
	digitalWrite(in3, HIGH);
	digitalWrite(in4, LOW);
	delay(2000);
	
	// Now change motor directions
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	delay(2000);
	
	// Turn off motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}

// This function lets you control speed of the motors
void speedControl() {
	// Turn on motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, HIGH);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	
	// Accelerate from zero to maximum speed
	
	
	// Now turn off motors
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}