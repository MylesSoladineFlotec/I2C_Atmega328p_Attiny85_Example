#include <TinyWireS.h>

bool readingAnalog = false;  // Flag to check if analog data is being read
int analogValue = 0;         // Variable to hold the analog value
#define ATtiny85_ADDRESS 0x04 // Define the I2C address for the ATtiny85
char lastCommand = ' ';      // Variable to store the last command received
int lastPin = 0;             // Variable to store the last pin

void setup() {
  TinyWireS.begin(ATtiny85_ADDRESS);  // Initialize TinyWireS with address
  TinyWireS.onRequest(requestEvent);  // Set up the request handler
}

void loop() {
  // Check if data is available from the master
  if (TinyWireS.available()) {
    receiveEvent();  // Process received data
  }

  // Continuously read analog data if readingAnalog is true
  if (readingAnalog) {
    analogValue = analogRead(A0);  // Read analog data
  }
}

// Function to handle received commands
void receiveEvent() {
  char command_letter = TinyWireS.read();  // Read the first character (command letter)

  if (command_letter == 'J' || command_letter == 'X') {
    char pinChar = TinyWireS.read();  // Read the second character (pin number)
    int pin = pinChar - '0';
    
    // Map '2' to PB3 (physical pin 2 on ATtiny85)
    if (pin == 2) {
      pin = 3;  // Map to PB3
    }
    
    lastPin = pin;  // Store the pin for feedback

    if (command_letter == 'J') {
      lastCommand = 'A';  // Confirmation for turning on solenoid
      turnOnSolenoid(pin);  // Turn on solenoid at the specific pin
    } else if (command_letter == 'X') {
      lastCommand = 'X';  // Confirmation for turning off solenoid
      turnOffSolenoid(pin);  // Turn off solenoid at the specific pin
    }
  } else if (command_letter == 'C') {
    char nextChar = TinyWireS.read();  // Read the next character after 'C'
    if (nextChar == 'E') {
      readingAnalog = false;  // Stop reading analog data
      lastCommand = 'E';  // Stop command
    } else {
      readingAnalog = true;  // Start reading analog data otherwise
      lastCommand = 'C';  // Start analog read command
    }
  }
}

// Function to handle I2C request events
void requestEvent() {
  if (readingAnalog) {
    // Send the 10-bit analog value (2 bytes)
    TinyWireS.write(analogValue & 0xFF);      // Low byte
    TinyWireS.write((analogValue >> 8) & 0x03); // High byte (only 2 bits for 10-bit ADC)
  } else {
    // Send the last pin number as a response
    TinyWireS.write(lastPin + '0');  // Send the pin number as ASCII
  }
}

// Function to turn on the solenoid at the specified pin
void turnOnSolenoid(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);  // Activate the solenoid (turn on PB3)
}

// Function to turn off the solenoid at the specified pin
void turnOffSolenoid(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);  // Deactivate the solenoid (turn off PB3)
}
