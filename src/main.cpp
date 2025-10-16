#include <Arduino.h>

// Pin definitions
const int BUTTON_PIN = D1;        // Button to trigger watering
const int RAIN_SENSOR_PIN = D2;   // Rain sensor digital output
const int PUMP_PIN = D5;          // Relay for water pump
const int VALVE_PIN = D6;         // Relay for valve

// Variables
bool lastButtonState = HIGH;      // Button with pull-up (HIGH when not pressed)
bool buttonPressed = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("\n=== Watering System Started ===");
  
  // Configure pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RAIN_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(VALVE_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Ensure pump and valve are off initially
  digitalWrite(PUMP_PIN, LOW);
  digitalWrite(VALVE_PIN, LOW);
  
  Serial.println("System ready. Press button to water.");
}

class ButtonManager {
  private:
    bool buttonPressed, isPumpOn, isValveOpen;
    unsigned long nextLogTime;
  
  void enablePumpAndValve() {
    digitalWrite(VALVE_PIN, HIGH);
    digitalWrite(PUMP_PIN, HIGH);
    isValveOpen = true;
    isPumpOn = true;
  }
  
  void disablePumpAndValve() {
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(VALVE_PIN, LOW);
    isValveOpen = false;
    isPumpOn = false;
  }
  
  public:
    ButtonManager() : buttonPressed(false), nextLogTime(0) {}

    void processPush() {
      bool currentButtonState = digitalRead(BUTTON_PIN);
      if (currentButtonState == LOW && buttonPressed == false) {
          buttonPressed = true;
          Serial.println("button pressed");
          digitalWrite(LED_BUILTIN, LOW);
          enablePumpAndValve();
      } else if (currentButtonState == LOW && buttonPressed == true) {
          buttonPressed = false;
          digitalWrite(LED_BUILTIN, HIGH);
          disablePumpAndValve();
          Serial.println("button released");
      }
      delay(200);
    }

    void processState() {
      bool rainSensorState = digitalRead(RAIN_SENSOR_PIN);
      if (isValveOpen == true && rainSensorState == LOW) {
        disablePumpAndValve();
      }
    }

    void publishState() {
      bool rainSensorState = digitalRead(RAIN_SENSOR_PIN);
      if (millis() > nextLogTime) {
        Serial.println("RAIN_SENSOR_PIN status is " + String(rainSensorState));
        nextLogTime = millis() + 2000;
      }
    }
};

enum ValveState {
    VALVE_CLOSED = 0,
    VALVE_OPEN = 1
};

enum PumpState {
    PUMP_OFF = 0,
    PUMP_ON = 1
};

//global instance
ButtonManager buttonManager;

void loop() {
  buttonManager.publishState();
  buttonManager.processPush();
  buttonManager.processState();
  
  delay(10); // Small delay for stability
}