#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd; // Declare LCD object

// Structure to hold incoming message
typedef struct {
  int buttonState;
} Message;

Message receivedMessage;

int pressCount = 0;  // Counter to track button presses

// New callback function with esp_now_recv_info parameter
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  // Copy the received data into our message structure
  memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

  // If the button was pressed (buttonState == 1), increment the press count
  if (receivedMessage.buttonState == 1) {
    pressCount++;  // Increment press count
    lcd.clear();
    lcd.print("Button Pressed:");
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(pressCount);
    Serial.print("Button pressed! Total count: ");
    Serial.println(pressCount);
  }
}

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  // Initialize LCD with I2C expander
  lcd.begin(16, 2);  // 16 columns and 2 rows
  lcd.clear();
  lcd.print("Receiver Ready");

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    lcd.clear();
    lcd.print("ESP-NOW Init Fail");
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register receive callback function
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("ESP-NOW Initialized");
}

void loop() {
  // The receiver doesn't need to do anything in the loop
  // All the work is done in the OnDataRecv callback function
}
