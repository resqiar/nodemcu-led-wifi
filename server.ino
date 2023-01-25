#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define LED1 D0
#define LED2 2

const char* ssid = "<ssid>";
const char* password = "<password>";

ESP8266WebServer server(8080);  // create a server on port 80

unsigned long currentMillis = millis();  // Store the current time
unsigned long previousMillis = 0;

bool blink1State = false;
bool blink2State = false;

int led1State = LOW;
int led2State = LOW;

// LED Blink speed
int blinkSpeed = 100;

void setup() {
  // initialize digital pins LED_BUILTIN as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    triggerLED();
  }

  Serial.println(WiFi.localIP());
  Serial.println("Connected to WiFi");

  server.on("/", handleRoot);  // register a request handler for the root path
  server.on("/status", handleStatus);
  server.on("/trigger", handleTriggerLED);
  server.on("/set-blink-speed", handleBlinkSpeed);

  server.begin();  // start the server
}

void loop() {
  server.handleClient();  // handle incoming requests

  currentMillis = millis();  // Update the current time

  if (currentMillis - previousMillis >= blinkSpeed) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    if (blink1State) {
      // if the LED is off turn it on and vice-versa:
      led1State = !led1State;
      // set the LED with the ledState of the variable:
      digitalWrite(LED1, led1State);
    }

    if (blink2State) {
      // if the LED is off turn it on and vice-versa:
      led2State = !led2State;
      // set the LED with the ledState of the variable:
      digitalWrite(LED2, led2State);
    }
  }
}

// request handler for the root path
void handleRoot() {
  server.send(200, "text/plain", "Hello World from NodeMCU!");
  triggerLED();
}

void handleStatus() {
  String led1 = (digitalRead(LED1) == HIGH ? "OFF" : "ON");
  String led2 = (digitalRead(LED2) == HIGH ? "OFF" : "ON");

  String json = "{\"LED1\":\"" + led1 + "\",\"LED2\":\"" + led2 + "\"}";

  server.send(200, "application/json", json);
}

void handleTriggerLED() {
  String led = server.arg("led");
  String state = server.arg("state");

  bool led1 = (led == "1" || led == "both");
  bool led2 = (led == "2" || led == "both");

  if (state == "on") {
    if (led1) digitalWrite(LED1, LOW);
    if (led2) digitalWrite(LED2, LOW);
    if (led1 && blink1State == HIGH) blink1State = false;
    if (led2 && blink2State == HIGH) blink2State = false;
    // blink1State = false;
    // blink2State = false;
  } else if (state == "off") {
    if (led1) digitalWrite(LED1, HIGH);
    if (led2) digitalWrite(LED2, HIGH);
    if (led1 && blink1State == HIGH) blink1State = false;
    if (led2 && blink2State == HIGH) blink2State = false;
    // blink1State = false;
    // blink2State = false;
  } else if (state == "blink") {
    if (led1) blink1State = led1;
    if (led2) blink2State = led2;
  }

  server.send(200);
}

void triggerLED() {
  digitalWrite(LED1, LOW);  // turn the LED on
  digitalWrite(LED2, LOW);  // turn the LED on
  delay(100);
  digitalWrite(LED1, HIGH);  // turn the LED off
  digitalWrite(LED2, HIGH);  // turn the LED off
  delay(100);
}

void handleBlinkSpeed() {
  triggerLED();
  String speed = server.arg("speed");  
  if(isNumeric(speed)) blinkSpeed = speed.toInt();
  server.send(200);  
}

bool isNumeric(String str) {
    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
