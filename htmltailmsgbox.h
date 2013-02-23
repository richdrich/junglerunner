/*
 * htmltailmsgbox.h
 *
 *  Created on: Feb 13, 2013
 *      Author: richardparratt
 */

#ifndef HTMLTAILMSGBOX_H_
#define HTMLTAILMSGBOX_H_

#include <stdlib.h>
#include <stdio.h>

#include "htmltailoption.h"
#include "usermessage.h"

#include "ctemps/msgbox.h"

class HtmltailMsgBox : public HtmltailOption {
public:
	HtmltailMsgBox(const char *text, int height, int width) : HtmltailOption()
	{
		parameters["TEXT"] = string(text);
		parameters["HEIGHT"] = height;
		parameters["WIDTH"] = width;
	}

	virtual int run() {

		string page = pageFromTemplate(msgbox);

		getPipe()->send(page.c_str());

		string response = getPipe()->receive();
		UserMessage msg = UserMessage::fromSerialized(response.c_str());

		fprintf(stderr, "setenv sequence %d\n", msg.sequence);
		writeSequence(boost::lexical_cast<string>(msg.sequence));

		if(msg.args.count("cancel")) {
			return -1;
		}


		return 0;
	}

private:


};


#endif /* HTMLTAILINFOBOX_H_ */
