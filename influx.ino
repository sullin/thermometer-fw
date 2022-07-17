#define IFXCLI_MIN_INTERVAL_MS  30000
#define IFX_TX_BUF    128

/* Send data to server */
void ifxcli_send_meas(String key, String val) {
  WiFiUDP udp;
  udp.beginPacket(conf.ifx_srv, conf.ifx_port);

  if (key.length() == 0 || val.length() == 0)
    return;

  String buf;
  buf += conf.ifx_db;

  if (conf.ifx_tags[0] != '\0') {
    buf += ",";
    buf += conf.ifx_tags;
  }
  buf += ",name=";
  buf += key;

  buf += " temperature=";
  buf += val;

  udp.write(buf.c_str(), buf.length());
  udp.endPacket();
}

void ifxcli_send() {
  String dat = thermometer_get(0);
  if (dat.length() == 0) return;

  int start = 0;
  int end;
  do {
    int eq = dat.indexOf("=", start);
    if (eq == -1) break;
    String key = dat.substring(start, eq);

    end = dat.indexOf("&", eq+1);
    if (end == -1) end = dat.length();
    String val = dat.substring(eq+1, end);

    ifxcli_send_meas(key, val);

    start = end+1;
  } while (end != -1);
}

unsigned long ifxcli_last = 0;

void ifxcli_init() {
  ifxcli_last = millis();
}

void ifxcli_task() {
  if (conf.ifx_srv[0] == '\0' || conf.ifx_port == 0 || conf.ifx_db[0] == '\0' || conf.ifx_interval_s == 0)
    return;

  unsigned long intrv = conf.ifx_interval_s * 1000;
  if (intrv < IFXCLI_MIN_INTERVAL_MS) return; // disabled

  unsigned long now = millis();
  if (now - ifxcli_last < intrv) return;
  ifxcli_last = now;

  pinMode(PIN_LED, INPUT);
  ifxcli_send();
  pinMode(PIN_LED, OUTPUT);
}
