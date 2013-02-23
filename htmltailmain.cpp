#include <stdlib.h>
#include <iostream>
#include <string>


#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <uriparser/Uri.h>

#include "ipcpipe.h"
#include "fcgiserver.h"
#include "htmltailinfobox.h"
#include "htmltailmsgbox.h"
#include "htmltailinputbox.h"

using namespace std;
using namespace boost::filesystem;


bool checkArgCount(int argc, int argRequired) {
	if(argc < argRequired) {
		fprintf(stderr, "%d arguments, need at least %d\n", argc, argRequired);
		return false;
	}

	return true;
}

int main(int argc, char *argv[]) {
	bool serverMode = false;

	enum whiptailCmdEnum { WT_INFOBOX, WT_MSGBOX, WT_INPUTBOX, WT_NULL=-1 };
	const char *whiptailOptions[] = { "--infobox", "--msgbox", "--inputbox", "" };

	enum whiptailCmdEnum doWhiptail=WT_NULL;
	int argStart=-1;

	for(int argi=1; argi<argc; argi++) {
		if(strcmp(argv[argi], "-D")==0 || strcmp(argv[argi], "--server")==0) {
			serverMode = true;
		}
		else {
			for(int wtoi=0; *whiptailOptions[wtoi]; wtoi++) {
				if(strcmp(argv[argi], whiptailOptions[wtoi])==0) {
					doWhiptail = (whiptailCmdEnum)wtoi;
					argStart = argi+1;
					break;
				}
			}
		}
	}

	// testing
	//IpcPipe::testme();
	//exit(0);

	if(serverMode) {
		(new FcgiServer())->execute();
	}
	else {
		int exitv=0;
		switch(doWhiptail) {
		case WT_INFOBOX:
			if(checkArgCount(argc, argStart+3)) {
				exitv = (new HtmltailInfoBox(argv[argStart], atoi(argv[argStart+1]), atoi(argv[argStart+2])))->run();
			}
			break;
		case WT_MSGBOX:
			if(checkArgCount(argc, argStart+3)) {
				exitv = (new HtmltailMsgBox(argv[argStart], atoi(argv[argStart+1]), atoi(argv[argStart+2])))->run();
			}
			break;
		case WT_INPUTBOX:
			if(checkArgCount(argc, argStart+3)) {

				string init;
				if(argStart+3 < argc && argv[argStart+3][0] != '-') {
					init = argv[argStart+3];
				}

				exitv = (new HtmltailInputBox(argv[argStart], atoi(argv[argStart+1]), atoi(argv[argStart+2]), init))->run();
			}
			break;
		case WT_NULL:
			fprintf(stderr, "Unrecognised option\n");
			break;
		}

		return exitv;
	}

    return 0;
}
