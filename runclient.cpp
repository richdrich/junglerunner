/*
 * runclient.cpp
 *
 *  Created on: Feb 24, 2013
 *      Author: richardparratt
 */

#include "runclient.h"

int RunClient::numCommandDefs = 3;
CommandDef RunClient::commandDefs[3] = {
	CommandDef("--msgbox", "SII", "", (const char *[]){"TEXT", "HEIGHT", "WIDTH", ""}, new HtmltailMsgBox()),
	CommandDef("--infobox", "SII", "", (const char *[]){"TEXT", "HEIGHT", "WIDTH", ""}, new HtmltailInfoBox()),
	CommandDef("--inputbox", "SII", "S", (const char *[]){"TEXT", "HEIGHT", "WIDTH", "INIT", ""}, new HtmltailInputBox())
};

int RunClient::numOptionDefs = 1;
OptionDef RunClient::optionDefs[] = {
	OptionDef("--ok-button", true, string("Ok"))
};
