/*
 * runclient.cpp
 *
 *  Created on: Feb 24, 2013
 *      Author: richardparratt
 */

#include "runclient.h"

int RunClient::numCommandDefs = 4;
CommandDef RunClient::commandDefs[4] = {
	CommandDef("--msgbox", "SII", "", 3, (const char *[]){"TEXT", "HEIGHT", "WIDTH", ""}, NULL, new HtmltailMsgBox()),
	CommandDef("--infobox", "SII", "", 3, (const char *[]){"TEXT", "HEIGHT", "WIDTH", ""}, NULL, new HtmltailInfoBox()),
	CommandDef("--inputbox", "SII", "S", 4, (const char *[]){"TEXT", "HEIGHT", "WIDTH", "INIT", ""}, NULL, new HtmltailInputBox()),
	CommandDef("--menu", "SIII", "SS*",
			6, (const char *[]){"TEXT", "HEIGHT", "WIDTH", "MENU_HEIGHT", "TAG", "ITEM", ""},
			(const char *[]){"", "", "", "", "MENU_ROWS", ""},new HtmltailMenu())
};

int RunClient::numOptionDefs = 1;
OptionDef RunClient::optionDefs[] = {
	OptionDef("--ok-button", true, string("Ok"))
};
