//////////////////////////////////////////////////////////////////////
// Pre-declarations of globals from httpserver
extern ESP8266WebServer server;
extern PageElement web_hdr_element;
extern PageElement web_ftr_element;

//////////////////////////////////////////////////////////////////////
const char page_conf[] = {
  "{{REQ}}"
  "<form method=\"post\"><table>"
  "<tr><td><h2>HTTP reporting</h2></td></tr>"
  "<tr><td>Server URL: </td><td><input type=\"text\" name=\"srv\" value=\"{{SRV}}\" /></td></tr>"
  "<tr><td>Token: </td><td><input type=\"text\" name=\"token\" value=\"{{TOKEN}}\" /></td></tr>"
  "<tr><td>Interval (s): </td><td><input type=\"number\" min=\"0\" max=\"300\" name=\"intrv\" value=\"{{INTRV}}\" /> (30-300, 0=disabled)</td></tr>"
  "<tr><td>Minimal temperature: </td><td><input type=\"number\" min=\"0\" max=\"100\" name=\"mint\" value=\"{{MINT}}\" /> (1-100, 0=disabled)</td></tr>"
  "<tr><td><h2>InfluxDB reporting</h2></td></tr>"
  "<tr><td>Server: </td><td><input type=\"text\" name=\"ifx_srv\" value=\"{{IFX_SRV}}\" /></td></tr>"
  "<tr><td>Port: </td><td><input type=\"number\" name=\"ifx_port\" value=\"{{IFX_PORT}}\" /></td></tr>"
  "<tr><td>DB: </td><td><input type=\"text\" name=\"ifx_db\" value=\"{{IFX_DB}}\" /></td></tr>"
  "<tr><td>Extra tags: </td><td><input type=\"text\" name=\"ifx_tags\" value=\"{{IFX_TAGS}}\" /></td></tr>"
  "<tr><td>Interval (s): </td><td><input type=\"number\" min=\"0\" max=\"300\" name=\"ifx_intrv\" value=\"{{IFX_INTRV}}\" /> (30-300, 0=disabled)</td></tr>"
  "<tr><td colspan=\"2\"><input type=\"submit\"/ name=\"submit\" value=\"Apply\"></td></tr>"
  "</table></form>"
};

/* Helper for HTML error message generation */
String err(const char *err) {
  String e = String("<div class=\"msg err\">");
  e += err;
  e += "</div>";
  return e;
}

/* Handle submit request if present; display result. */
String token_REQ(PageArgument& args) {
  struct config conf2 = conf;
  if (!args.hasArg("submit")) return "";
  
  if (args.hasArg("srv")) {
    if (args.arg("srv").length() >= sizeof(conf2.url))
      return err("Invalid server url");
    strcpy(conf2.url, args.arg("srv").c_str());
  }
  if (args.hasArg("token")) {
    if (args.arg("token").length() >= sizeof(conf2.token))
      return err("Invalid server token");
    strcpy(conf2.token, args.arg("token").c_str());
  }
  if (args.hasArg("intrv")) {
    int intrv = atoi(args.arg("intrv").c_str());
    if (intrv != 0 && (intrv < 30 || intrv > 300))
      return err("Invalid interval");
    conf2.interval_s = intrv;
  }
  if (args.hasArg("mint")) {
    int mint = atoi(args.arg("mint").c_str());
    if (mint < 0 || mint > 100)
      return err("Invalid minimal temperature");
    conf2.mintemp = mint;
  }

  if (args.hasArg("ifx_srv")) {
    if (args.arg("ifx_srv").length() >= sizeof(conf2.ifx_srv))
     return err("Invalid influx server");
   strcpy(conf2.ifx_srv, args.arg("ifx_srv").c_str());
  }
  if (args.hasArg("ifx_port")) {
    int ifx_port = atoi(args.arg("ifx_port").c_str());
    if (ifx_port != 0 && ifx_port > 65535)
      return err("Invalid influx port");
    conf2.ifx_port = ifx_port;
  }
  if (args.hasArg("ifx_db")) {
    if (args.arg("ifx_db").length() >= sizeof(conf2.ifx_db))
      return err("Invalid influx db");
    strcpy(conf2.ifx_db, args.arg("ifx_db").c_str());
  }
  if (args.hasArg("ifx_tags")) {
    if (args.arg("ifx_tags").length() >= sizeof(conf2.ifx_tags))
      return err("Invalid influx tags");
    strcpy(conf2.ifx_tags, args.arg("ifx_tags").c_str());
  }
  if (args.hasArg("ifx_intrv")) {
    int ifx_intrv = atoi(args.arg("ifx_intrv").c_str());
    if (ifx_intrv != 0 && (ifx_intrv < 30 || ifx_intrv > 3600))
      return err("Invalid influx interval");
    conf2.ifx_interval_s = ifx_intrv;
  }

  conf = conf2;
  conf_store();
  return  String("<div class=\"msg ok\">Configuration saved</div>");
}

/* Configuration form field value getters */
String token_SRV(PageArgument& args) {
  return String(conf.url);
}
String token_TOKEN(PageArgument& args) {
  return String(conf.token);
}
String token_INTRV(PageArgument& args) {
  return String(conf.interval_s);
}
String token_MINT(PageArgument& args) {
  return String(conf.mintemp);
}

String token_IFX_SRV(PageArgument& args) {
  return String(conf.ifx_srv);
}
String token_IFX_PORT(PageArgument& args) {
  return String(conf.ifx_port);
}
String token_IFX_DB(PageArgument& args) {
  return String(conf.ifx_db);
}
String token_IFX_TAGS(PageArgument& args) {
  return String(conf.ifx_tags);
}
String token_IFX_INTRV(PageArgument& args) {
  return String(conf.ifx_interval_s);
}

PageElement conf_page_element(page_conf, {
  {"REQ", token_REQ},
  {"SRV", token_SRV},
  {"TOKEN", token_TOKEN},
  {"INTRV", token_INTRV},
  {"MINT", token_MINT},

  {"IFX_SRV", token_IFX_SRV},
  {"IFX_PORT", token_IFX_PORT},
  {"IFX_DB", token_IFX_DB},
  {"IFX_TAGS", token_IFX_TAGS},
  {"IFX_INTRV", token_IFX_INTRV},
});

PageBuilder conf_page("/conf", {
  web_hdr_element,
  conf_page_element,
  web_ftr_element
});

void conf_page_init() {
  conf_page.insert(server);
}
