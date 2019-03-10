#define HTTPCLI_MIN_INTERVAL_MS  30000

/* Send data to server */
void httpcli_send() {
    WiFiClient client;
    HTTPClient http;

    String data = thermometer_get(true);
    if (data.length() == 0) return;

    if (http.begin(client, conf.url)) {
      http.setUserAgent("Thermometer/" VERSION);
      http.addHeader("Authorization", conf.token);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      int ret = http.POST((uint8_t*)data.c_str(), data.length());
      if (ret == HTTP_CODE_OK) {
        Serial.printf("[http] Data sent\n");
      } else {
        Serial.printf("[http] Failed to send data\n");
      }
      
      http.end();
    } else {
      Serial.printf("[http] Unable to connect\n");
    }
}

unsigned long httpcli_last = 0;

void httpcli_init() {
  httpcli_last = millis();
}

void httpcli_task() {
  unsigned long intrv = conf.interval_s * 1000;
  if (intrv < HTTPCLI_MIN_INTERVAL_MS) return; // disabled
  
  unsigned long now = millis();
  if (now - httpcli_last < intrv) return;
  httpcli_last = now;

  pinMode(PIN_LED, INPUT);
  httpcli_send();
  pinMode(PIN_LED, OUTPUT);
}
