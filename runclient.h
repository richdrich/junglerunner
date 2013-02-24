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

class RunClient {
public:
	RunClient(int argc, char *argv[]) {
		this->argc = argc;
		this->argv = argv;

		//const char *whiptailOptions[] = { "--infobox", "--msgbox", "--inputbox", "" };

		argStart = -1;
		pActiveCommand = NULL;
		//doWhiptail = WT_NULL;
		for(int argi=1; argi<argc; argi++) {
			for(int wtoi=0; wtoi<numCommandDefs; wtoi++) {
				if(strcmp(argv[argi], commandDefs[wtoi].option)==0) {
					pActiveCommand = &commandDefs[wtoi];
					argStart = argi+1;
					break;
				}
			}
		}
	}

	int run() {
		if(!checkArgCount(argStart + strlen(pActiveCommand->reqArgs))) {
			return -1;
		}

		map<string, variant<string, int> > params;
		bool optsDone=false;
		for(uint ai=0; pActiveCommand->argNames[ai][0]; ai++) {
			if((argStart + ai) >= (uint)argc || argv[argStart+ai][0]=='-') {
				optsDone = true;
			}

			char argType = ai < strlen(pActiveCommand->reqArgs)
				? pActiveCommand->reqArgs[ai]
				: pActiveCommand->optArgs[ai - strlen(pActiveCommand->reqArgs)];

			if(argType=='S') {
				if(optsDone) {
					params[pActiveCommand->argNames[ai]] = string();
				}
				else {
					params[pActiveCommand->argNames[ai]] = string(argv[argStart+ai]);
				}
			}
			else if(argType=='I') {
				if(optsDone) {
					params[pActiveCommand->argNames[ai]] = 0;
				}
				else {
					params[pActiveCommand->argNames[ai]] = lexical_cast<int>(string(argv[argStart+ai]));
				}
			}
		}

		pActiveCommand->optionClass->addParameters(params);

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

	int argc;
	char **argv;

	// enum whiptailCmdEnum doWhiptail;
	uint argStart;
	CommandDef *pActiveCommand;

};


#endif /* RUNCLIENT_H_ */
