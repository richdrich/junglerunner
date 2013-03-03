/*
 * commanddef.h
 *
 *  Created on: Feb 24, 2013
 *      Author: richardparratt
 */

#ifndef COMMANDDEF_H_
#define COMMANDDEF_H_

#include <vector>

using namespace std;

class CommandDef {
public:
	CommandDef(const char *option, const char *reqArgs, const char *optArgs,
			int numNames, const char * argNames[], const char *sections[],
			HtmltailOption *optionClass) {
		this->option = option;
		this->reqArgs = reqArgs;
		this->optArgs = optArgs;
		this->argNames = argNames;

		if(sections==NULL) {
			for(int n=0; n<numNames; n++) {
				this->sections.push_back("");
			}
		}
		else {
			string lastSection;
			for(int n=0; n<numNames; n++) {
				if(*sections[n]) {
					lastSection = sections[n];
				}
				this->sections.push_back(lastSection);
			}
		}

		this->optionClass = optionClass;
	}

	const char *option;
	const char *reqArgs;
	const char *optArgs;
	const char **argNames;
	vector <string> sections;
	HtmltailOption *optionClass;
};


#endif /* COMMANDDEF_H_ */
