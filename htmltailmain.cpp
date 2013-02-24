#include <stdlib.h>
#include <iostream>
#include <string>


#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <uriparser/Uri.h>

#include "ipcpipe.h"
#include "fcgiserver.h"
#include "runclient.h"

using namespace std;
using namespace boost::filesystem;




int main(int argc, char *argv[]) {
	bool serverMode = false;

	for(int argi=1; argi<argc; argi++) {
		if(strcmp(argv[argi], "-D")==0 || strcmp(argv[argi], "--server")==0) {
			serverMode = true;
		}
	}

	// testing
	//IpcPipe::testme();
	//exit(0);

	if(serverMode) {
		(new FcgiServer())->execute();
	}
	else {
		RunClient runClient(argc, argv);
		return runClient.run();
	}

    return 0;
}
