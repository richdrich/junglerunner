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

		string page = pageFromTemplate(getTemplateText().c_str());

		getPipe()->send(page.c_str());

		return 0;
	}

private:

};


#endif /* HTMLTAILINFOBOX_H_ */
