#include <WiFi.h>
#include <WiFiClient.h>
#include "credentials.h"
#define URL "GET /"HEALTHCHECKS" HTTP/1.0"

WiFiClient https;

bool pingHealthchecks()
{
  char status[32] = {0};
  if (!https.connect("hc-ping.com", 80))
  {
    Serial.println("Connection to hc-ping.com failed");
    https.stop();
    return false;
  }

  Serial.println("Connected to hc-ping.com");
  https.println(URL);
  https.println("Host: hc-ping.com");
  https.println("Connection: close");
 
  if (https.println() == 0)
  {
    Serial.println("Failed to ping healthchecks");
    https.stop();
    return false;
  }

  https.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print("Unexpected response from healthchecks: ");
    Serial.println(status);
    https.stop();
    return false;
  }  
}

String translateEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

void scanNetworks() {
  int numberOfNetworks = WiFi.scanNetworks();
 
  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);
 
  for (int i = 0; i < numberOfNetworks; i++) {
 
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
 
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
 
    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));
 
    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
  }
}
