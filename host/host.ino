#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define POMMES_DECREMENT_PIN 15  // Button to decrement Pommes count
#define SCHNITZEL_DECREMENT_PIN 4 // Button to decrement Schnitzel count

hd44780_I2Cexp lcd; // Declare LCD object

// Structure to hold incoming message
typedef struct {
  int pommesButtonState;
  int schnitzelButtonState;
} Message;

Message receivedMessage;

int pommesCount = 0;   // Counter to track "Pommes" button presses
int schnitzelCount = 0;  // Counter to track "Schnitzel" button presses

// New callback function with esp_now_recv_info parameter
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  // Copy the received data into our message structure
  memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));

  // If the "Pommes" button was pressed (pommesButtonState == 1), increment the Pommes count
  if (receivedMessage.pommesButtonState == 1) {
    pommesCount++;  // Increment Pommes count
    Serial.print("Pommes button pressed! Total count: ");
    Serial.println(pommesCount);
  }

  // If the "Schnitzel" button was pressed (schnitzelButtonState == 1), increment the Schnitzel count
  if (receivedMessage.schnitzelButtonState == 1) {
    schnitzelCount++;  // Increment Schnitzel count
    Serial.print("Schnitzel button pressed! Total count: ");
    Serial.println(schnitzelCount);
  }

  // Update the LCD display with both counts
  lcd.clear();
  lcd.print("Pommes: ");
  lcd.print(pommesCount);
  lcd.setCursor(0, 1);
  lcd.print("Schnitzel: ");
  lcd.print(schnitzelCount);
}

void setup() {
  // Initialize Serial Monitor for debugging
  Serial.begin(115200);

  // Initialize LCD with I2C expander
  lcd.begin(16, 2);  // 16 columns and 2 rows
  lcd.clear();
  lcd.print("Receiver Ready");

  // Set up decrement buttons
  pinMode(POMMES_DECREMENT_PIN, INPUT_PULLUP);
  pinMode(SCHNITZEL_DECREMENT_PIN, INPUT_PULLUP);

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
  // Check if Pommes decrement button is pressed
  if (digitalRead(POMMES_DECREMENT_PIN) == LOW) {
    if (pommesCount > 0) {
      pommesCount--;  // Decrement Pommes count
    }
    Serial.print("Pommes decrement button pressed! New count: ");
    Serial.println(pommesCount);

    // Update the LCD display
    lcd.clear();
    lcd.print("Pommes: ");
    lcd.print(pommesCount);
    lcd.setCursor(0, 1);
    lcd.print("Schnitzel: ");
    lcd.print(schnitzelCount);

    delay(500);  // Simple debounce delay
  }

  // Check if Schnitzel decrement button is pressed
  if (digitalRead(SCHNITZEL_DECREMENT_PIN) == LOW) {
    if (schnitzelCount > 0) {
      schnitzelCount--;  // Decrement Schnitzel count
    }
    Serial.print("Schnitzel decrement button pressed! New count: ");
    Serial.println(schnitzelCount);

    // Update the LCD display
    lcd.clear();
    lcd.print("Pommes: ");
    lcd.print(pommesCount);
    lcd.setCursor(0, 1);
    lcd.print("Schnitzel: ");
    lcd.print(schnitzelCount);

    delay(500);  // Simple debounce delay
  }

  // The receiver doesn't need to do anything else in the loop
}
