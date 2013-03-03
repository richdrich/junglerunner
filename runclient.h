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
#include "htmltailmenu.h"
#include "commanddef.h"
#include "optiondef.h"
#include "optionset.h"
#include "debug.h"

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

		uint argDefIdx=0;
		for(uint argIdx=argStart; argIdx < (uint)argc; argIdx++) {

			if(argv[argIdx][0]=='-') {
				break;
			}

			bool optional = argDefIdx >= strlen(pActiveCommand->reqArgs);
			char argType = optional
				? pActiveCommand->optArgs[argDefIdx - strlen(pActiveCommand->reqArgs)]
				: pActiveCommand->reqArgs[argDefIdx];

			debugf( "process argIdx=%d, argDefIdx=%d, optional=%s, argType=%c, arg=%s\n",
				argIdx, argDefIdx, optional ? "t" : "f", argType, 	pActiveCommand->argNames[argDefIdx]);

			if(argType=='*') {
				argDefIdx = strlen(pActiveCommand->reqArgs);
				argIdx--;
				continue;
			}

			if(argType=='S') {
				if(optsDone) {
					addParam(params, string(pActiveCommand->argNames[argDefIdx]), ParamEntry("", optional, pActiveCommand->sections[argDefIdx]));
				}
				else {
					addParam(params, string(pActiveCommand->argNames[argDefIdx]), ParamEntry(argv[argIdx], optional, pActiveCommand->sections[argDefIdx]));
				}
			}
			else if(argType=='I') {
				if(optsDone) {
					addParam(params, string(pActiveCommand->argNames[argDefIdx]), ParamEntry(0, optional, pActiveCommand->sections[argDefIdx]));
				}
				else {
					addParam(params, string(pActiveCommand->argNames[argDefIdx]),
							ParamEntry(lexical_cast<int>(string(argv[argIdx])), optional, pActiveCommand->sections[argDefIdx]));
				}
			}

			argDefIdx++;

		}

		pActiveCommand->optionClass->addParameters(params);
		pActiveCommand->optionClass->setOptions(optionSettings);
		pActiveCommand->optionClass->setCommandName(string(&pActiveCommand->option[2]));
		return pActiveCommand->optionClass->run();
	}

private:
	void addParam(map<string, ParamEntry > &params, string name, struct ParamEntry ent) {
		if(params.count(name)) {
			params[name].add(ent.values);
		}
		else {
			params[name] = ent;
		}
	}

	bool checkArgCount(int argRequired) {
		if(argc < argRequired) {
			debugf( "%d arguments, need at least %d\n", argc, argRequired);
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
