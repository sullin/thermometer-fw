
/* Configuration struct */
struct config {
  uint32_t magic;
  uint8_t checksum;
  uint8_t connected;
  char url[64];
  char token[16];
  uint16_t interval_s; // 30-300; 0-disabled
  uint8_t mintemp; // 0-disabled

  char ifx_srv[64];
  uint16_t ifx_port;
  char ifx_db[16];
  char ifx_tags[32];
  uint16_t ifx_interval_s; // 30-300; 0-disabled
};

/* Singleton copy of running configuration */
extern struct config conf;

void conf_load();
void conf_store();
void config_reset();
