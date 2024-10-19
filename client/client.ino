#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> 

#define BUTTON_PIN 15

hd44780_I2Cexp lcd; 

typedef struct {
  int buttonState;
} Message;

Message message;

// MAC address of the receiver
uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0xDF, 0xF2, 0xE8};

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  lcd.clear();
  if (status == ESP_NOW_SEND_SUCCESS) {
    lcd.print("Sent: Success");
    Serial.println("Message sent successfully!");
  } else {
    lcd.print("Sent: Fail");
    Serial.println("Failed to send message!");
  }
}

void setup() {
  Serial.begin(115200);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Sender Ready");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);  // Ensure Wi-Fi station mode

  if (esp_now_init() != ESP_OK) {
    lcd.clear();
    lcd.print("Error initializing");
    Serial.println("Error initializing ESP-NOW");
    while (true);
  } else {
    Serial.println("ESP-NOW Initialized");
  }

  esp_now_register_send_cb(OnDataSent);

  // Add peer (receiver)
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  // Clear peer info structure
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer and check success
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    lcd.clear();
    lcd.print("Failed to add peer");
    Serial.println("Failed to add peer");
  } else {
    Serial.println("Peer added successfully");
  }
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW) {
    message.buttonState = 1;
    Serial.println("Button pressed!");

    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));

    lcd.clear();
    if (result == ESP_OK) {
      lcd.print("Sending...");
      Serial.println("Message is being sent...");
    } else {
      lcd.print("Error Sending");
      Serial.println("Error Sending Message");
    }

    delay(1000);  // Debounce
  } else {
    message.buttonState = 0;
  }
}
