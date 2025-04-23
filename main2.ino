#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <UniversalTelegramBot.h>

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram Bot Credentials
const char* botToken = "YOUR_TELEGRAM_BOT_TOKEN";
const String chatID = "YOUR_TELEGRAM_CHAT_ID";

// Soil Moisture Sensor Pin
#define SOIL_SENSOR_PIN A0

// LED Pin
#define LED_PIN D6

// Moisture Threshold (wet soil = higher value)
#define MOISTURE_THRESHOLD 600

// Google Form URL
const String form_url = "https://docs.google.com/forms/d/e/1FAIpQLSfDd6iS2fdrZGoplZcmpQcx1uPxzt_fmdJW0znIc9ZoVC1NWQ/formResponse?usp=pp_url&entry.1309902686=";

// Secure WiFi Client
WiFiClientSecure secureClient;
UniversalTelegramBot bot(botToken, secureClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(LED_PIN, OUTPUT);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  secureClient.setInsecure();
}

void loop() {
  int moistureValue = analogRead(SOIL_SENSOR_PIN);
  Serial.print("Soil Moisture Value: ");
  Serial.println(moistureValue);

  // LED ON when soil is wet (moisture value is HIGH)
  if (moistureValue > MOISTURE_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  // Send to Google Form
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String finalURL = form_url + String(moistureValue) + "&submit=Submit";
    http.begin(secureClient, finalURL);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("✅ Data sent to Google Form!");
    } else {
      Serial.print("❌ Google Form failed. HTTP code: ");
      Serial.println(httpCode);
    }
    http.end();
  }

  // Send Telegram Message
  String message = "📤 Soil Moisture Report\n";
  message += "💧 Moisture Level: " + String(moistureValue) + "\n";
  message += "📅 Time: " + String(millis() / 1000) + "s since start";
  bot.sendMessage(chatID, message, "");

  delay(5000);
}
