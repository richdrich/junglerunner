/*
 * htmltailinputbox.h
 *
 *  Created on: Feb 13, 2013
 *      Author: richardparratt
 */

#ifndef HTMLTAILINPUTBOX_H_
#define HTMLTAILINPUTBOX_H_

#include <stdlib.h>
#include <stdio.h>

#include "htmltailoption.h"
#include "usermessage.h"


class HtmltailInputBox : public HtmltailOption {
public:
	HtmltailInputBox() : HtmltailOption() {

	}

	virtual int processResultMsg(UserMessage msg) {
		fputs(msg.args["input"].c_str(), stderr);

		return HtmltailOption::processResultMsg(msg);
	}
private:


};


#endif /* HTMLTAILINFOBOX_H_ */
