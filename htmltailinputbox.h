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

#include "ctemps/inputbox.h"

class HtmltailInputBox : public HtmltailOption {
public:
	HtmltailInputBox() : HtmltailOption() {

	}

//	HtmltailInputBox(const char *text, int height, int width, string init) : HtmltailOption()
//	{
//		parameters["TEXT"] = string(text);
//		parameters["HEIGHT"] = height;
//		parameters["WIDTH"] = width;
//		parameters["INIT"] = init;
//	}

	virtual int run() {

		string page = pageFromTemplate(inputbox);

		getPipe()->send(page.c_str());

		string response = getPipe()->receive();
		UserMessage msg = UserMessage::fromSerialized(response.c_str());

		// fprintf(stderr, "setenv sequence %d\n", msg.sequence);
		writeSequence(boost::lexical_cast<string>(msg.sequence));

		fputs(msg.args["input"].c_str(), stderr);

		if(msg.args.count("cancel")) {
			return -1;
		}


		return 0;
	}

private:


};


#endif /* HTMLTAILINFOBOX_H_ */
