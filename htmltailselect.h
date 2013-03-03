/*
 * htmltailselect.h
 *
 *  Created on: Feb 13, 2013
 *      Author: richardparratt
 */

#ifndef HTMLTAILSELECT_H_
#define HTMLTAILSELECT_H_

#include <stdlib.h>
#include <stdio.h>

#include "htmltailoption.h"
#include "usermessage.h"


class HtmltailSelect : public HtmltailOption {
public:
	HtmltailSelect() : HtmltailOption() {

	}

	virtual int processResultMsg(UserMessage msg) {
		fputs(msg.args["select"].c_str(), stderr);

		return HtmltailOption::processResultMsg(msg);
	}
private:


};


#endif /* HTMLTAILSELECT_H_ */
