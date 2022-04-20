//////////////////////////////////////////////////////////////////////
// Pre-declarations of globals from httpserver
extern ESP8266WebServer server;
extern PageElement web_hdr_element;
extern PageElement web_ftr_element;

//////////////////////////////////////////////////////////////////////
const char page_conf[] = {
  "{{REQ}}"
  "<form method=\"post\"><table>"
  "<tr><td>Server: </td><td><input type=\"text\" name=\"srv\" value=\"{{SRV}}\" /></td></tr>"
  "<tr><td>Token: </td><td><input type=\"text\" name=\"token\" value=\"{{TOKEN}}\" /></td></tr>"
  "<tr><td>Interval (s): </td><td><input type=\"number\" min=\"0\" max=\"300\" name=\"intrv\" value=\"{{INTRV}}\" /> (30-300, 0=disabled)</td></tr>"
  "<tr><td>Minimal temperature: </td><td><input type=\"number\" min=\"0\" max=\"100\" name=\"mint\" value=\"{{MINT}}\" /> (1-100, 0=disabled)</td></tr>"
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
  if (!args.hasArg("submit")) return "";
  
  if (!args.hasArg("srv") || args.arg("srv").length() >= sizeof(conf.url))
    return err("Invalid server url");
  if (!args.hasArg("token") || args.arg("token").length() >= sizeof(conf.token))
    return err("Invalid server token");
  if (!args.hasArg("intrv"))
    return err("Missing interval");
  int intrv = atoi(args.arg("intrv").c_str());
  if (intrv != 0 && (intrv < 30 || intrv > 300))
    return err("Invalid interval");
  if (!args.hasArg("mint"))
    return err("Missing minimal temperature");
  int mint = atoi(args.arg("mint").c_str());
  if (mint < 0 || mint > 100)
    return err("Invalid minimal temperature");

  strcpy(conf.url, args.arg("srv").c_str());
  strcpy(conf.token, args.arg("token").c_str());
  conf.interval_s = intrv;
  conf.mintemp = mint;
  
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

PageElement conf_page_element(page_conf, {
  {"REQ", token_REQ},
  {"SRV", token_SRV},
  {"TOKEN", token_TOKEN},
  {"INTRV", token_INTRV},
  {"MINT", token_MINT},
});

PageBuilder conf_page("/conf", {
  web_hdr_element,
  conf_page_element,
  web_ftr_element
});

void conf_page_init() {
  conf_page.insert(server);
}
