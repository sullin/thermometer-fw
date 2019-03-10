#define CONF_MAGIC 0x38437522

struct config conf;

/* Load config from EEPROM, on error read defaults */
void conf_load() {
  uint8_t *c = (uint8_t*) &conf;
  uint8_t i;
  uint8_t chk = 0;
  EEPROM.begin(sizeof(struct config));
  for(i=0; i<sizeof(struct config); i++) {
    c[i] = EEPROM.read(i);
    chk += c[i];
  }
  EEPROM.end();
  conf.url[sizeof(conf.url)-1] = '\0';
  
  // check for magic and checksum validity
  if (conf.magic == CONF_MAGIC && chk == 0) return;

  // defaults
  for(i=0; i<sizeof(struct config); i++)
    c[i] = 0;
  conf.mintemp = 50;
}

/* Store configuration to EEPROM */
void conf_store() {
  uint8_t i;
  conf.url[sizeof(conf.url)-1] = '\0';
  
  // Update magic and checksum
  conf.magic = CONF_MAGIC;
  uint8_t *c = (uint8_t*) &conf;
  uint8_t chk = 0;
  for(i=0; i<sizeof(struct config); i++)
    chk += c[i];
  conf.checksum -= chk;
  
  EEPROM.begin(sizeof(struct config));
  for(i=0; i<sizeof(struct config); i++)
    EEPROM.write(i, c[i]);
  EEPROM.end();
}

/* Reset configuration */
void config_reset() {
  EEPROM.begin(sizeof(struct config));
  EEPROM.write(0, 0xFF);
  EEPROM.end();
  conf_load();
}
