ESP8266WebServer server;

//////////////////////////////////////////////////////////////////////
/* Web page header; used for all web pages served by us. */
const char web_hdr[] PROGMEM = {
  "<html><head>\n"
  "<title>Thermometer</title>\n"
  "<style>\n"
  "  body { background: #fff; font-family: sans-serif; Color: #000; }\n"
  "  .msg { width: 100% }\n"
  "  .ok { background: #88ff88 }\n"
  "  .err { background: #ff8888 }\n"
  "  td { border-bottom: 1px #eee dotted; padding: 5px 15px; }\n"
  "</style>\n"
  "</head>\n"
  "<body>\n"
  "<h1>Thermometer</h1>\n"
  "<a href=\"/\">Status</a></li> - \n"
  "<a href=\"/conf\">Configuration</a></li>\n"
  "<hr>\n"
};
PageElement web_hdr_element(web_hdr, {});

/* Web page footer. */
const char web_ftr[] PROGMEM = {
  "</body></html>\n"
};
PageElement web_ftr_element(web_ftr, {});

//////////////////////////////////////////////////////////////////////
/* Status page */
const char page_root[] PROGMEM = {
  "<div id=\"temp\"></div>\n"
  "\n"
  "<script type=\"text/javascript\">\n"
  "  function http_get(fn, onload) {\n"
  "    var req = new XMLHttpRequest();\n"
  "    req.onreadystatechange = function() {\n"
  "      if (req.readyState != 4) return;\n"
  "      if (req.status == 200) {\n"
  "        onload(req.responseText);\n"
  "      } else {\n"
  "        onload(null);\n"
  "      }\n"
  "    };\n"
  "    req.open('GET', fn, true);\n"
  "    req.send();\n"
  "  }\n"
  "  function display(s) {\n"
  "    if (s !== null) {\n"
  "      var a = s.split('&');\n"
  "      var out = '<table>';\n"
  "      for (var i of a) {\n"
  "        var j = i.split('=');\n"
  "        if (j.length != 2) continue;\n"
  "        out += '<tr><td>'+j[0]+'</td><td>'+j[1]+' &deg;C</td></tr>';\n"
  "      }\n"
  "      out += '</table>';\n"
  "      document.getElementById(\"temp\").innerHTML = out;\n"
  "    }\n"
  "    setTimeout(update, 500);\n"
  "  }\n"
  "  function update() {\n"
  "    http_get(\"/data\", display);\n"
  "  }\n"
  "  update();\n"
  "</script>\n"
};
PageElement root_page_element(page_root, {});
PageBuilder root_page("/", {
  web_hdr_element,
  root_page_element,
  web_ftr_element
});

//////////////////////////////////////////////////////////////////////
/* Temperature data page */
String token_DATA(PageArgument& args) {
  return thermometer_get(false);
}
PageElement data_page_element("{{DATA}}", {
  {"DATA", token_DATA},
});
PageBuilder data_page("/data", {
  data_page_element
});

//////////////////////////////////////////////////////////////////////
void httpserver_init() {
  root_page.insert(server);
  data_page.insert(server);
  server.begin();
}

void httpserver_task() {
  server.handleClient();
}
