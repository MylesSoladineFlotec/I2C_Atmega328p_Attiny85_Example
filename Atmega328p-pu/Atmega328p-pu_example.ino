#include <Wire.h>

#define ATtiny85_ADDRESS 0x04 // I2C Address of ATtiny85

bool isReadingAnalog = false;  // Flag to control continuous analog reading

void setup() {
  Serial.begin(9600);   // Start the serial monitor
  Wire.begin();         // Join I2C bus as master
  Wire.setClock(100000);
}

void loop() {
  // Check if a command has been sent from the serial monitor
  if (Serial.available() > 0) {
    String command = Serial.readString(); // Read the entire command
    command.trim(); // Remove any trailing whitespace

    processCommand(command);  // Handle the command based on input
  }

  // Continuously read analog values if isReadingAnalog is true
  if (isReadingAnalog) {
    readAnalogValues();  // Function to request and read analog values
    delay(500);          // Add delay to avoid spamming I2C requests
  }
}

// Function to handle commands received over Serial
void processCommand(String command) {
  if (command.startsWith("J") || command.startsWith("X")) {
    char command_letter = command.charAt(0); // 'J' or 'X'
    char pin = command.charAt(1);            // Pin number character
    sendCommandToATtiny(command_letter, pin); // Send solenoid command to ATtiny85
    requestReceivedPin(); // Request and print the received pin number or response
  }
  else if (command == "C") { // Command to start analog reading
    isReadingAnalog = true;
    sendCommandToATtiny('C', ' ');  // Inform ATtiny85 to start reading analog
    Serial.println("Start reading analog values...");
  }
  else if (command == "CE") { // Command to stop analog reading
    isReadingAnalog = false;
    sendCommandToATtiny('C', 'E');  // Inform ATtiny85 to stop reading analog
    Serial.println("Stop reading analog values.");
  }
  else if (command == "R") { // Command to request a single analog value
    requestAnalogValue();  // Request a one-time analog value from ATtiny85
  }
}

// Function to send commands to ATtiny85 over I2C
void sendCommandToATtiny(char command_letter, char pin) {
  Wire.beginTransmission(ATtiny85_ADDRESS); // Begin I2C transmission
  Wire.write(command_letter);               // Send command ('J', 'X', or 'C')
  Wire.write(pin);                          // Send the pin number or 'E' for stopping analog read
  Wire.endTransmission();                   // End I2C transmission
}

// Function to request and print the pin number or last command from ATtiny85
void requestReceivedPin() {
  Wire.requestFrom(ATtiny85_ADDRESS, 1);  // Request 1 byte from ATtiny85
  if (Wire.available()) {
    char receivedChar = Wire.read();  // Read the received pin number or last command
    Serial.print("Received from ATtiny85: ");
    Serial.println(receivedChar);     // Print the received character
  }
}

// Function to continuously request and read analog values from ATtiny85
void readAnalogValues() {
  requestAnalogValue();  // Request analog value from ATtiny85
}

// Function to request and read the analog value from ATtiny85
void requestAnalogValue() {
  Wire.requestFrom(ATtiny85_ADDRESS, 2);  // Request 2 bytes from ATtiny85
  
  if (Wire.available() == 2) {            // If 2 bytes are available
    int lowByte = Wire.read();            // Read the low byte
    int highByte = Wire.read();           // Read the high byte
    int analogValue = (highByte << 2) | lowByte;  // Combine bytes to get the 10-bit value
    
    Serial.print("Analog Value: ");
    Serial.println(analogValue);          // Print the analog value
  } else {
    Serial.println("Failed to read analog value.");
  }
}
