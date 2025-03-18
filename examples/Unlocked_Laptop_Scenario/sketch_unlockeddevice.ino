#include <USB.h>
#include <USBHIDKeyboard.h>
#include <Arduino.h>

USBHIDKeyboard keyboard;

const int actionDelay = 2000;
const int typingDelay = 100;

const int buttonPin = 0;
bool previousButtonState = HIGH;
bool sequenceRunning = false;

void setup() {
  Serial.begin(115200);
  Serial.println("LILYGO T-Dongle-S3 USB HID Keyboard");
  
  pinMode(buttonPin, INPUT_PULLUP);
  
  USB.begin();
  keyboard.begin();

  delay(1000);
  Serial.println("USB HID Keyboard initialized");
  Serial.println("Press the button to start the sequence");
}

void loop() {
  bool currentButtonState = digitalRead(buttonPin);
  
  if (previousButtonState == HIGH && currentButtonState == LOW && !sequenceRunning) {
    Serial.println("Button pressed, starting sequence...");
    sequenceRunning = true;
    delay(500);

    performKeyboardSequence();
    
    sequenceRunning = false;
    Serial.println("Sequence completed");
  }
  
  previousButtonState = currentButtonState;
}

void performKeyboardSequence() {
  delay(actionDelay);

  Serial.println("Opening web browser...");
  keyboard.press(KEY_LEFT_GUI);
  keyboard.press('r');
  keyboard.releaseAll();
  delay(actionDelay);
 
  typeString("chrome");
  keyboard.press(KEY_RETURN);
  keyboard.release(KEY_RETURN);
  delay(actionDelay * 2);
  
  Serial.println("Navigating to GitHub raw file page...");
  typeString("https://github.com/ladysecspeare/lilygo-t-dongle-s3-project/blob/main/examples/Unlocked_Laptop_Scenario/test%20file.txt");
  keyboard.press(KEY_RETURN);
  keyboard.release(KEY_RETURN);
  delay(actionDelay * 3);
  
  Serial.println("Pressing Ctrl + Shift + S to download file...");
  keyboard.press(KEY_LEFT_CTRL);
  keyboard.press(KEY_LEFT_SHIFT);
  keyboard.press('s');
  keyboard.releaseAll();
  delay(actionDelay);

  Serial.println("Download sequence completed");
}

void typeString(const char* str) {
  for (size_t i = 0; i < strlen(str); i++) {
    keyboard.write(str[i]);
    delay(typingDelay);
  }
}
