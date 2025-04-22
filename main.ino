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
#define SOIL_SENSOR_PIN A0  // Analog pin for HW-080

// Google Form URL (adjust the field ID as needed)
const String form_url = "https://docs.google.com/forms/d/e/1FAIpQLSfDd6iS2fdrZGoplZcmpQcx1uPxzt_fmdJW0znIc9ZoVC1NWQ/formResponse?usp=pp_url&entry.1309902686=";

// Secure WiFi Client
WiFiClientSecure secureClient;
UniversalTelegramBot bot(botToken, secureClient);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    secureClient.setInsecure(); // Not recommended for production
}

void loop() {
    int moistureValue = analogRead(SOIL_SENSOR_PIN);
    Serial.print("Soil Moisture Value: ");
    Serial.println(moistureValue);

    // === 1. Send to Google Form ===
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
    } else {
        Serial.println("⚠️ WiFi not connected!");
    }

    // === 2. Send Telegram Message ===
    String message = "📤 Soil Moisture Report\n";
    message += "💧 Moisture Level: " + String(moistureValue) + "\n";
    message += "📅 Time: " + String(millis() / 1000) + "s since start";

    bool sent = bot.sendMessage(chatID, message, "");
    if (sent) {
        Serial.println("✅ Telegram message sent!");
    } else {
        Serial.println("❌ Failed to send Telegram message.");
    }

    delay(5000);  // Run every 5 seconds
}
