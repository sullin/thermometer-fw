#include <ESP8266WiFi.h>        // ESP8266 wifi support
#include <DNSServer.h>          // DNS server, needed by WifiManager
#include <ESP8266WebServer.h>   // HTTP server
#include <PageBuilder.h>
#include <ESP8266HTTPClient.h>  // HTTP client

#include <WiFiManager.h>        // Wifi autoconnect portal

#include <SPI.h>                // SPI interface support
#include <OneWire.h>            // 1-wire support
#include <DallasTemperature.h>  // DS28B20 1-wire temperature sensor support
#include <EEPROM.h>             // EEPROM support for configuration storage

#include "config.h"             // configuration structure definition

/* STATUS LED:
 *  red: WiFi not configured, AP with configuration portal running
 *  blinking red: connecting to wifi
 *  green: ready
 *  blinking green: transmitting
 *  off: configuration portal timeout
 */
#define PIN_LED D4

/* WIFI DE-PAIR BUTTON:
 *  After system has booted, hold down the button to reset wifi pairing information.
 */
#define PIN_BTN D3
#define BTN_LONGPRESS_MS  2500

#define VERSION "0.1"

/* Reset wifi configuration */
void do_config_reset() {
  Serial.printf("Resetting configuration\n");
  config_reset();

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW); // red

  // wait for btn release
  while(digitalRead(PIN_BTN) == LOW) yield();

  WiFi.disconnect(false);
  while (WiFi.status() == WL_CONNECTED) yield();
  
  // wait for contact bounce to end to avoid booting to UART mode
  delay(500);
  // reboot to start wifi config portal
  ESP.restart();
}

/* Button debounce task */
bool btn_state = false;
unsigned long btn_state_change = 0;
void btn_task() {
  if (digitalRead(PIN_BTN) == LOW) {
    if (!btn_state) btn_state_change = millis();
    if (millis() - btn_state_change > BTN_LONGPRESS_MS) {
      do_config_reset();
    }
    btn_state = true;
  } else {
    btn_state = false;
  }
}

/* Wait for wifi connection, blink led, poll button */
void waitWifiConnection() {
  Serial.printf("Waiting for WiFi connection\n");
  int led = 1;
  while (WiFi.status() != WL_CONNECTED) {
    int i;
    for (i=0; i<5 && WiFi.status() != WL_CONNECTED; i++) {
      btn_task();
      delay(100);
    }
    // blink red led while connecting
    pinMode(PIN_LED, led?OUTPUT:INPUT);
    led = !led;
  }
}

/* Wifi autoconnect portal */
void autoconnect() {
  if (!conf.connected) {
    Serial.printf("Starting AutoConnect\n");
    WiFi.disconnect(false);
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(5*60);
    wifiManager.setConnectTimeout(5);
    if (!wifiManager.startConfigPortal("Thermometer")) {
      Serial.println("failed to connect");
      pinMode(PIN_LED, INPUT);
      // When the user did not complete registration within timeout, just wait here.
      // We do not want to leave unsecured APs standing around
      while(1) yield();
    }

    // Save configuration done flag. SSID and password are stored by ESP core itself
    conf.connected = 1;
    conf_store();
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }
}

void setup() {

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Thermometer v." VERSION "\n");

  conf_load();

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW); // red

  autoconnect();
  waitWifiConnection();
  Serial.printf("Connected to %s, local IP %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); // green
  
  httpserver_init();
  httpcli_init();
  conf_page_init();
  thermometer_init();
}

void loop() {
  httpserver_task();
  httpcli_task();
  btn_task();
}
