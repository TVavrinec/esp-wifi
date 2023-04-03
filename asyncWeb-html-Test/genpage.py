#!/usr/bin/env python

import json

html = open("src/index.html", "r")
h = open("src/webpage.h", "w")

h.write("#pragma once\n\n")
h.write("const char index_html[] PROGMEM =\"")

for line in html:
    h.write(json.dumps(line).strip('"').replace("\r", ""))

h.write("\";\n")

h.close()
html.close()