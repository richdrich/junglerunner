/*
 * commanddef.h
 *
 *  Created on: Feb 24, 2013
 *      Author: richardparratt
 */

#ifndef COMMANDDEF_H_
#define COMMANDDEF_H_

class CommandDef {
public:
	CommandDef(const char *option, const char *reqArgs, const char *optArgs, const char * argNames[], HtmltailOption *optionClass) {
		this->option = option;
		this->reqArgs = reqArgs;
		this->optArgs = optArgs;
		this->argNames = argNames;
		this->optionClass = optionClass;
	}

	const char *option;
	const char *reqArgs;
	const char *optArgs;
	const char **argNames;
	HtmltailOption *optionClass;
};


#endif /* COMMANDDEF_H_ */
