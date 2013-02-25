#ifndef ALLTEMPLATES_H
#define ALLTEMPLATES_H
static const char * AllTemplates[][2] = {
{ "msgbox", 
"<html>\n"
"<head><title>Prompt</title></head>\n"
"\n"
"<body>\n"
"<form action='{{SCRIPT}}' method='get'>\n"
"<h2>\n"
"<input type='hidden' name='seq' value='{{SEQ}}' />\n"
"<input type='hidden' name='id' value='{{ID}}' />\n"
"{{TEXT}}\n"
"</h2>\n"
"<p><input type='submit' name='cancel' value='Cancel' /><input type='submit' name='submit' value='Ok' /></p>\n"
"</form>\n"
"</body>\n"
"</html>\n"
}
,
{ "inputbox", 
"<html>\n"
"<head><title>Input</title>\n"
"<link rel=\"stylesheet\" type=\"text/css\" href=\"/resource/default.css\">\n"
"</head>\n"
"\n"
"<body>\n"
"<form action='{{SCRIPT}}' method='get'>\n"
"<div class='main'>\n"
"<div class='content'>\n"
"<input type='hidden' name='seq' value='{{SEQ}}' />\n"
"<input type='hidden' name='id' value='{{ID}}' />\n"
"<div class='prompt gentext'>{{TEXT}}</div>\n"
"<input type='textbox' class='boxtext' name='input' value='{{INIT}}' />\n"
"\n"
"<div class='buttons'>\n"
"<div class='leftbutton'><input type='submit' name='cancel' value='Cancel' /></div>\n"
"<div class='rightbutton'><input type='submit' name='submit' value='Ok' /></div>\n"
"</div>\n"
"</div>\n"
"</div>\n"
"</form>\n"
"</body>\n"
"</html>\n"
}
,
{ "infobox", 
"<html><head><title>Info</title>\n"
"<link rel=\"stylesheet\" type=\"text/css\" href=\"/resource/default.css\">\n"
"</head>\n"
"\n"
"<body>\n"
"<div class='main'>\n"
"<div class='content text gentext'>{{TEXT}}</div>\n"
"<div>\n"
"</body></html>\n"
}
};
const int NumTemplates=3;
#endif