
#define PIN_ONE_WIRE  D0
#define PIN_ADC_CS    D2
#define PIN_TC_CS     D1

#define W1_DELAY      1000
#define W1_INTRV      10000

OneWire oneWire(PIN_ONE_WIRE);
DallasTemperature DS18B20(&oneWire);

#define W1_CNT      10

bool min_ok;

void thermometer_init() {
  /* Initialize SPI controller */
  SPI.pins(D5, D6, D7, D2);
  SPI.setDataMode(0);
  SPI.setFrequency(1000000);
  SPI.begin();

  /* SPI CS pins */
  pinMode(PIN_ADC_CS, OUTPUT);
  pinMode(PIN_TC_CS, OUTPUT);
  digitalWrite(PIN_ADC_CS, HIGH);
  digitalWrite(PIN_TC_CS, HIGH);

  /* 1-wire */
  pinMode(PIN_ONE_WIRE, INPUT_PULLUP);
  DS18B20.begin();
  DS18B20.setCheckForConversion(false);
  DS18B20.setResolution(9);
}

/* Read 1-wire temperatures */
String thermometer_get_w1() {
  String ret;
  float tempC;
  
  DS18B20.requestTemperatures();

  int i = 0;
  DeviceAddress da;
  while (DS18B20.getAddress(da, i++)) {
    float c = DS18B20.getTempC((uint8_t*) da);
    if (c > conf.mintemp) min_ok = true;

    char tmp[32];
    snprintf(tmp, sizeof(tmp), "w1_%02x%02x%02x%02x%02x%02x%02x%02x=%.2f&",
      da[0], da[1], da[2], da[3], da[4], da[5], da[6], da[7], c);
    ret += tmp;
  }
  return ret;
}

/* Read thermocouple temperatures */
String thermometer_get_tc() {
  String ret;
  uint8_t d[4];
  char tmp[32];

  digitalWrite(PIN_TC_CS, LOW);
  SPI.transfer(d, 4);
  digitalWrite(PIN_TC_CS, HIGH);

  int16_t tcref = (d[2]<<8) | (d[3] & 0xF0);
  int16_t tc = (d[0]<<8) | (d[1] & 0xFC);

  if ((d[1] & 0x01) == 0) { // if error bit not set
    float t = tc / 16.0;
    if (t > conf.mintemp) min_ok = true;
    snprintf(tmp, sizeof(tmp), "tc=%.2f&", t);
    ret += tmp;
  }

  float t = tcref / 256.0;
  if (t > conf.mintemp) min_ok = true;
  snprintf(tmp, sizeof(tmp), "tcref=%.2f&", t);
  ret += tmp;
  
  return ret;
}

/* Read ADC-connected NTC thermistor temperatures */
#define ADC_R0  10000.0   // pull-up resistor
#define NTC_T   25.0      // thermistor T0

String thermometer_get_adc(uint8_t ch) {
  String ret;
  uint8_t d[2];
  char tmp[32];

  d[0] = 0xD0 | (ch << 5);
  d[1] = 0;

  digitalWrite(PIN_ADC_CS, LOW);
  SPI.transfer(d, 2);
  digitalWrite(PIN_ADC_CS, HIGH);

  uint16_t a = ((d[0] & 0x07) << 7) | (d[1] >> 1);

  /* Sanity check: -35 to 150C */
  if (a > 990 || a < 20) return "";

  float t = ADC_R0 / (1024.0 / a - 1);
  t = log(t / (float)(conf.ntc_r25)) / (float)(conf.ntc_beta) + 1.0 / (NTC_T + 273.15);  // 1/B * ln(R/Ro) + (1/To)
  t = 1/t - 273.15;

  if (t > conf.mintemp) min_ok = true;
  snprintf(tmp, sizeof(tmp), "ntc%d=%.2f&", ch, t);
  ret += tmp;
  
  return ret;
}

String thermometer_get(bool check_min) {
  String ret;
  min_ok = false;

  ret += thermometer_get_w1();
  ret += thermometer_get_tc();
  ret += thermometer_get_adc(0);
  ret += thermometer_get_adc(1);

  // remove trailing &
  if (ret.length() > 0) {
    ret.remove(ret.length()-1, 1);
  }

  if (check_min && conf.mintemp > 0 && !min_ok) return "";
  return ret;
}
