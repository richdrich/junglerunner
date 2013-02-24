/*
 * htmltailinfobox.h
 *
 *  Created on: Feb 13, 2013
 *      Author: richardparratt
 */

#ifndef HTMLTAILINFOBOX_H_
#define HTMLTAILINFOBOX_H_

#include "htmltailoption.h"

class HtmltailInfoBox : public HtmltailOption {
public:
	HtmltailInfoBox() : HtmltailOption()
	{
	}

	virtual int run() {
		const char * tplText = "<html><head><title>Info</title></head>\n"
			"<body><h2>"
			"{{TEXT}}"
			"</h2></body></html>";

		string page = pageFromTemplate(tplText);

		getPipe()->send(page.c_str());

		return 0;
	}

private:

};


#endif /* HTMLTAILINFOBOX_H_ */
