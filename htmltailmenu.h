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
		ParamEntry tags = parameters["TAG"];

		for(vector<boost::variant<string, int> >::iterator tagValue=tags.values.begin(); tagValue!=tags.values.end(); tagValue++) {
			string strTag = boost::lexical_cast<string>(*tagValue);
			if(msg.args.count(strTag)) {
				fputs(strTag.c_str(), stderr);
				break;
			}
		}


		return HtmltailOption::processResultMsg(msg);
	}

};


#endif /* HTMLTAILMENU_H_ */
