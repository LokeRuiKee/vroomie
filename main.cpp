#include <Servo.h>

Servo Myservo;

// Define pins
#define trigPin 9
#define echoPin 8
#define MLa 4
#define MLb 5
#define MRa 6
#define MRb 7
#define joystickX A0
#define joystickY A1
#define modeSwitch 2 // Pin for the mode switch

long duration, distance;
int xValue, yValue;
boolean autonomousMode = false;

void setup() {
  Serial.begin(9600);

  // Set pin modes
  pinMode(MLa, OUTPUT);
  pinMode(MLb, OUTPUT);
  pinMode(MRa, OUTPUT);
  pinMode(MRb, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(modeSwitch, INPUT_PULLUP); // Use internal pull-up resistor

  Myservo.attach(10);
}

void loop() {
  // Read mode switch
  boolean switchState = digitalRead(modeSwitch); 
  if (switchState == LOW) { // Switch is pressed, enter autonomous mode
    autonomousMode = true;
  } else {
    autonomousMode = false;
  }

  if (autonomousMode) {
    // Autonomous mode logic
    getDistance();
     // Obstacle avoidance logic
    if (distance > 15) {               // Condition For Absence Of Obstacle            
        Myservo.write(90);
        digitalWrite(MRb, HIGH);       // Move Forward
        digitalWrite(MRa, LOW);
        digitalWrite(MLb, HIGH);                                
        digitalWrite(MLa, LOW);                                                       
    }
    else if ((distance < 10)&&(distance > 0)) {            // Condition For Presence Of Obstacle
        digitalWrite(MRb, LOW);     //Stop                
        digitalWrite(MRa, LOW);
        digitalWrite(MLb, LOW);                                
        digitalWrite(MLa, LOW);
        delay(100);
    
        Myservo.write(0);
        delay(500);
        Myservo.write(180);
        delay(500);
        Myservo.write(90);
        delay(500);
    
        digitalWrite(MRb, LOW);     // Move Backward             
        digitalWrite(MRa, HIGH);
        digitalWrite(MLb, LOW);                                
        digitalWrite(MLa, HIGH);
        delay(500);
        digitalWrite(MRb, LOW);        //Stop                
        digitalWrite(MRa, LOW);
        digitalWrite(MLb, LOW);                                
        digitalWrite(MLa, LOW);  
        delay(100);  
        digitalWrite(MRb, HIGH);     // Move Left     
        digitalWrite(MRa, LOW);   
        digitalWrite(MLa, LOW);                                 
        digitalWrite(MLb, LOW);  
        delay(500);
    }
    else {
        // Move forward
        digitalWrite(MRb, HIGH);
        digitalWrite(MRa, LOW);
        digitalWrite(MLb, HIGH);
        digitalWrite(MLa, LOW);
    }
  } else {
    // User control mode
    readJoystick();
    controlMotors();
  }

  delay(10);
}

void readJoystick() {
  xValue = analogRead(joystickX);
  yValue = analogRead(joystickY);
}

void controlMotors() {
  // Map joystick Y-axis to forward/backward speed
  int speed = map(yValue, 0, 1023, -255, 255); 

  // Adjust speeds for turning based on joystick X-axis
  int leftSpeed = speed;
  int rightSpeed = speed;

  if (xValue > 512) { // Turn right
    rightSpeed -= map(xValue - 512, 0, 511, 0, 128); 
  } else if (xValue < 512) { // Turn left
    leftSpeed -= map(512 - xValue, 0, 511, 0, 128); 
  }

  // Constrain speeds to the valid range (-255 to 255)
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // Motor A (Left)
  if (leftSpeed > 0) { 
    digitalWrite(MLa, LOW); // Forward
    digitalWrite(MLb, HIGH);
  } else if (leftSpeed < 0) {
    digitalWrite(MLa, HIGH); // Backward
    digitalWrite(MLb, LOW);
  } else {
    digitalWrite(MLa, LOW); // Stop
    digitalWrite(MLb, LOW); 
  }
  analogWrite(9, abs(leftSpeed)); // PWM control for speed

  // Motor B (Right)
  if (rightSpeed > 0) {
    digitalWrite(MRa, LOW); // Forward
    digitalWrite(MRb, HIGH);
  } else if (rightSpeed < 0) {
    digitalWrite(MRa, HIGH); // Backward
    digitalWrite(MRb, LOW);
  } else {
    digitalWrite(MRa, LOW); // Stop
    digitalWrite(MRb, LOW);
  }
  analogWrite(10, abs(rightSpeed)); // PWM control for speed
}

void getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.2;
}