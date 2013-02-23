#ifndef inputbox_H
#define inputbox_H
const char * inputbox = 
"<html>\n"
"<head><title>Input</title></head>\n"
"\n"
"<body>\n"
"<form action='{{SCRIPT}}' method='get'>\n"
"<h2>\n"
"<input type='hidden' name='seq' value='{{SEQ}}' />\n"
"<input type='hidden' name='id' value='{{ID}}' />\n"
"{{TEXT}}\n"
"<input type='textbox' name='input' value='{{INIT}}' />\n"
"</h2>\n"
"<p><input type='submit' name='cancel' value='Cancel' /><input type='submit' name='submit' value='Ok' /></p>\n"
"</form>\n"
"</body>\n"
"</html>\n"
;
#endif
