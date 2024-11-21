/**************************************************************************
HTTP Request Example
Fetch a random emoji from emojihub.yurace.pro and display it on the screen.

To fetch a new emoji, press either button on the LILYGO (GPIO 0 or 35).
**************************************************************************/
#include "TFT_eSPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// TODO: replace with your own SSID & Password
const char* ssid = "Barnard Guest";
const char* password = "";

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35

volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  // Setup the display
  tft.init();
  tft.setRotation(1);  // 1 = landscape, 2 = portrait
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  Serial.print("Display dimensions: ");
  Serial.print(tft.width());
  Serial.print(" x ");
  Serial.println(tft.height());

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Fetch initial emoji
  if (WiFi.status() == WL_CONNECTED) {
    fetchEmoji();
  } else {
    Serial.println("WiFi not connected");
  }

  // Setup buttons
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), pressedLeftButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), pressedRightButton, FALLING);
}

void fetchEmoji() {
  HTTPClient http;

  String url = "http://emojihub.yurace.pro/api/random";
  http.begin(url);

  // Send the GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Response: " + response);

    JSONVar responseJSON = JSON.parse(response);

    if (JSON.typeof(responseJSON) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    // Extract and display the emoji data
    String emoji = responseJSON["htmlCode"][0]; // Unicode HTML code
    String name = (const char*)responseJSON["name"];
    String category = (const char*)responseJSON["category"];

    Serial.println("Emoji: " + emoji);
    Serial.println("Name: " + name);
    Serial.println("Category: " + category);

    // Display the emoji and metadata on the screen
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(10, 30);
    tft.println("Emoji: " + emoji);
    tft.setCursor(10, 60);
    tft.println("Name: " + name);
    tft.setCursor(10, 90);
    tft.println("Category: " + category);

  } else {
    Serial.println("Error on sending GET request");
  }

  // Free resources
  http.end();
}

void pressedLeftButton() {
  leftButtonPressed = true;
}

void pressedRightButton() {
  rightButtonPressed = true;
}

void loop() {
  // Fetch emoji when either button is pressed
  if (leftButtonPressed) {
    fetchEmoji();
    leftButtonPressed = false;
  }
  if (rightButtonPressed) {
    fetchEmoji();
    rightButtonPressed = false;
  }
}