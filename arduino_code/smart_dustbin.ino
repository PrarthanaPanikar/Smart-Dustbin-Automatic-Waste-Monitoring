#include <Servo.h>

// =====================================================
// SMART DUSTBIN - OPTION A
// Arduino UNO + HC-SR04 + Servo + LED + Buzzer
// Virtual Simulation using Wokwi
// =====================================================

// -------------------- Pin Definitions -----------------

const int TRIG_PIN = 7;
const int ECHO_PIN = 6;

const int SERVO_PIN = 9;

const int BUZZER_PIN = 8;
const int LED_PIN = 13;

// -------------------- Servo Settings ------------------

Servo lidServo;

const int LID_CLOSED_ANGLE = 0;
const int LID_OPEN_ANGLE = 90;

// -------------------- Detection Settings --------------

const float HAND_DISTANCE = 15.0;     
const float FULL_BIN_DISTANCE = 5.0;  

const unsigned long LID_OPEN_TIME = 3000;

// -------------------- System Variables ----------------

bool lidOpen = false;
bool fullAlert = false;

unsigned long lidOpenedTime = 0;

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  // Sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Output pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Servo initialization
  lidServo.attach(SERVO_PIN);
  lidServo.write(LID_CLOSED_ANGLE);

  // Initial output state
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  // Startup message
  Serial.println("================================");
  Serial.println("       SMART DUSTBIN SYSTEM");
  Serial.println("       OPTION A - WOKWI");
  Serial.println("================================");
  Serial.println("System Started...");
  Serial.println();
}

// =====================================================
// ULTRASONIC DISTANCE FUNCTION
// =====================================================

float getDistance() {

  // Ensure trigger is LOW
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send 10 microsecond ultrasonic pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Sensor timeout
  if (duration == 0) {
    return -1;
  }

  // Convert time into distance
  float distance = duration * 0.0343 / 2.0;

  return distance;
}

// =====================================================
// OPEN LID
// =====================================================

void openLid() {

  if (!lidOpen) {

    lidServo.write(LID_OPEN_ANGLE);

    lidOpen = true;
    lidOpenedTime = millis();

    Serial.println(">>> OBJECT DETECTED");
    Serial.println(">>> LID OPENED");
  }
}

// =====================================================
// CLOSE LID
// =====================================================

void closeLid() {

  if (lidOpen) {

    lidServo.write(LID_CLOSED_ANGLE);

    lidOpen = false;

    Serial.println(">>> LID CLOSED");
  }
}

// =====================================================
// FULL BIN ALERT
// =====================================================

void activateFullBinAlert() {

  if (!fullAlert) {

    fullAlert = true;

    // Turn ON red LED
    digitalWrite(LED_PIN, HIGH);

    // Generate audible buzzer tone
    tone(BUZZER_PIN, 1000);

    Serial.println();
    Serial.println("!!! FULL BIN ALERT !!!");
    Serial.println("!!! RED LED ON !!!");
    Serial.println("!!! BUZZER ON - 1kHz !!!");
    Serial.println();
  }
}

// =====================================================
// NORMAL STATUS
// =====================================================

void deactivateFullBinAlert() {

  if (fullAlert) {

    fullAlert = false;

    // Turn OFF LED
    digitalWrite(LED_PIN, LOW);

    // Stop buzzer
    noTone(BUZZER_PIN);

    Serial.println(">>> BIN STATUS NORMAL");
    Serial.println(">>> ALERT OFF");
  }
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Read ultrasonic sensor
  float distance = getDistance();

  // ---------------------------------------------------
  // SENSOR ERROR
  // ---------------------------------------------------

  if (distance < 0) {

    Serial.println("Sensor Error: No Echo Received");

    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);

    delay(500);

    return;
  }

  // ---------------------------------------------------
  // DISPLAY DISTANCE
  // ---------------------------------------------------

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // ---------------------------------------------------
  // FULL BIN DETECTION
  // ---------------------------------------------------

  if (distance <= FULL_BIN_DISTANCE) {

    activateFullBinAlert();

  }

  // ---------------------------------------------------
  // NORMAL BIN CONDITION
  // ---------------------------------------------------

  else {

    deactivateFullBinAlert();
  }

  // ---------------------------------------------------
  // HAND / OBJECT DETECTION
  // ---------------------------------------------------

  if (distance <= HAND_DISTANCE &&
      distance > FULL_BIN_DISTANCE) {

    openLid();
  }

  // ---------------------------------------------------
  // AUTOMATIC LID CLOSING
  // ---------------------------------------------------

  if (lidOpen &&
      millis() - lidOpenedTime >= LID_OPEN_TIME) {

    closeLid();
  }

  // ---------------------------------------------------
  // STABILITY DELAY
  // ---------------------------------------------------

  delay(200);
}