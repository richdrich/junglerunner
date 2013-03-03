/*
 * htmltailmenu.h
 *
 *  Created on: Feb 27, 2013
 *      Author: richardparratt
 */

#ifndef HTMLTAILMENU_H_
#define HTMLTAILMENU_H_

class HtmltailMenu : public HtmltailOption {
public:
	HtmltailMenu() : HtmltailOption() {

	}


protected:
	virtual int processResultMsg(UserMessage msg) {

		fputs(msg.args["input"].c_str(), stderr);

		return HtmltailOption::processResultMsg(msg);
	}

};


#endif /* HTMLTAILMENU_H_ */
