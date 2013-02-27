/*
 * runclient.h
 *
 *  Created on: Feb 24, 2013
 *      Author: richardparratt
 */

#ifndef RUNCLIENT_H_
#define RUNCLIENT_H_

#include <string>

using namespace std;

#include "htmltailinfobox.h"
#include "htmltailmsgbox.h"
#include "htmltailinputbox.h"
#include "commanddef.h"
#include "optiondef.h"
#include "optionset.h"

class RunClient {
public:
	RunClient(int argc, char *argv[]) {
		this->argc = argc;
		this->argv = argv;

		argStart = -1;
		pActiveCommand = NULL;

		for(int opti=0; opti<numOptionDefs; opti++) {
			optionSettings[optionDefs[opti].name] = OptionSet(optionDefs[opti].defaultValue, true);
		}

		for(int argi=1; argi<argc; argi++) {
			if(argv[argi][0]=='-' && argv[argi][1]=='-') {

				for(int wtoi=0; wtoi<numCommandDefs; wtoi++) {
					if(strcmp(argv[argi], commandDefs[wtoi].option)==0) {
						pActiveCommand = &commandDefs[wtoi];
						argStart = argi+1;
						break;
					}
				}

				for(int opti=0; opti<numOptionDefs; opti++) {
					if(strcmp(argv[argi], optionDefs[opti].name.c_str())==0) {
						if(optionDefs[opti].hasText) {
							optionSettings[optionDefs[opti].name] = OptionSet(string(argv[++argi]), false);
						}
						else {
							optionSettings[optionDefs[opti].name] = OptionSet(true, false);
						}
					}
				}
			}
		}
	}

	int run() {
		if(!checkArgCount(argStart + strlen(pActiveCommand->reqArgs))) {
			return -1;
		}

		map<string, ParamEntry > params;
		bool optsDone=false;
		for(uint ai=0; pActiveCommand->argNames[ai][0]; ai++) {
			bool optional = (argStart + ai) >= (uint)argc;
			if(optional || argv[argStart+ai][0]=='-') {
				optsDone = true;
			}

			char argType = ai < strlen(pActiveCommand->reqArgs)
				? pActiveCommand->reqArgs[ai]
				: pActiveCommand->optArgs[ai - strlen(pActiveCommand->reqArgs)];

			if(argType=='S') {
				if(optsDone) {
					params[pActiveCommand->argNames[ai]] = ParamEntry("", optional);
				}
				else {
					params[pActiveCommand->argNames[ai]] = ParamEntry(argv[argStart+ai], optional);
				}
			}
			else if(argType=='I') {
				if(optsDone) {
					params[pActiveCommand->argNames[ai]] = ParamEntry(0, optional);
				}
				else {
					params[pActiveCommand->argNames[ai]]
					       = ParamEntry(lexical_cast<int>(string(argv[argStart+ai])), optional);
				}
			}
		}

		pActiveCommand->optionClass->addParameters(params);
		pActiveCommand->optionClass->setOptions(optionSettings);
		pActiveCommand->optionClass->setCommandName(string(&pActiveCommand->option[2]));
		return pActiveCommand->optionClass->run();
	}

private:
	bool checkArgCount(int argRequired) {
		if(argc < argRequired) {
			fprintf(stderr, "%d arguments, need at least %d\n", argc, argRequired);
			return false;
		}

		return true;
	}

	// enum whiptailCmdEnum { WT_INFOBOX, WT_MSGBOX, WT_INPUTBOX, WT_NULL=-1 };

	static int numCommandDefs;
	static CommandDef commandDefs[];
	static int numOptionDefs;
	static OptionDef optionDefs[];

	int argc;
	char **argv;

	// enum whiptailCmdEnum doWhiptail;
	uint argStart;
	CommandDef *pActiveCommand;
	map <string, OptionSet> optionSettings;

};


#endif /* RUNCLIENT_H_ */
