/*
 * fcgiserver.h
 *
 *  Created on: Feb 13, 2013
 *      Author: richardparratt
 */

#ifndef FCGISERVER_H_
#define FCGISERVER_H_

#include <iostream>
#include <string>
#include <sys/types.h>
#include <signal.h>

#include "fcgio.h"
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <uriparser/Uri.h>

#include "ipcpipe.h"
#include "usermessage.h"
#include "processinfo.h"

class FcgiServer;

class RunShellArgs {
public:
	RunShellArgs(FcgiServer *pThis, string path, string id) {
		this->pThis = pThis;
		this->path = path;
		this->id = id;
	}

	FcgiServer *pThis;
	string path;
	string id;
};

class ReadIncomingArgs {
public:
	ReadIncomingArgs(FcgiServer *pThis, IpcPipe *pipe) {
		this->pThis = pThis;
		this->pipe = pipe;
	}

	FcgiServer *pThis;
	IpcPipe *pipe;
};

struct EventInfo {
	boost::condition_variable cond;
	boost::mutex lockMutex;
	bool occurred;
};

class FcgiServer {
public:
	FcgiServer() {

	}

	void execute() {


		// The following turned out to not be needed
		//		if(::siginterrupt(SIGINT, 1) < 0) {
		//			perror("siginterrupt");
		//		}

	    // Backup the stdio streambufs
	    streambuf * cin_streambuf  = cin.rdbuf();
	    streambuf * cout_streambuf = cout.rdbuf();

	    // Listen on port 8000 - may not be needed
	    int sockfd = FCGX_OpenSocket(":8000", 1024);

	    FCGX_Init();

	    // process requests in a loop
	    // after accepting a request, it will be handled by threads
	    // and finished when either a message is received or
	    // the underlying shell script terminates
	    while (true) {
			// Put a handler on SIGUSR1
			// If we don't do this, the signal will run around the threads terminating them
			// and making the next accept fail
			struct sigaction act, old;

			act.sa_handler=sigUsr1Handler;
			sigemptyset(&act.sa_mask);
			if(::sigaction(SIGUSR1, &act, &old) < 0) {
				perror("sigaction");
			}

		    FCGX_Request *pRequest = new FCGX_Request();
		    FCGX_InitRequest(pRequest, sockfd, 0);

		    fprintf(stderr, "Waiting for request\n");
		    int acceptres = FCGX_Accept_r(pRequest);

	    	if(acceptres < 0) {
	    		fprintf(stderr, "FCGX_Accept_r failed returned %d, errno=%d\n", acceptres, errno);
	    		break;
	    	}

	        fcgi_streambuf cin_fcgi_streambuf(pRequest->in);
	        fcgi_streambuf cout_fcgi_streambuf(pRequest->out);

	        cin.rdbuf(&cin_fcgi_streambuf);
	        cout.rdbuf(&cout_fcgi_streambuf);

	        // Parse the GET query and put the id tag in 'id'
	        // and all other params into UserMessage.args
	        const char *qstring = FCGX_GetParam("QUERY_STRING", pRequest->envp);
	        fprintf(stderr, "Processing request, qstring=%s\n", qstring);

	        ::UriQueryListA *queryList;
	        int numparams;
	        string id;
	        int reqSeq;
	        UserMessage userMessage;

	        int res = ::uriDissectQueryMallocA(&queryList, &numparams, qstring, &qstring[strlen(qstring)]);
	        if(res==URI_SUCCESS) {
	        	for(UriQueryListStructA *pqi=queryList; NULL != pqi; pqi = pqi->next) {
	        		if(strcmp(pqi->key, "id")==0) {
	        			id = string(pqi->value);
	        		}
	        		else if(strcmp(pqi->key, "seq")==0) {
	        			reqSeq = ::atoi(pqi->value);
	        			userMessage.sequence = reqSeq + 1;
	        		}
	        		else {
	        			userMessage.args[pqi->key] = pqi->value;
	        		}
	        	}
	        }
	        uriFreeQueryListA(queryList);

	        // Get the shell script to run
	        string shellPath = string(FCGX_GetParam("SCRIPT_FILENAME", pRequest->envp));

//	        char **env = pRequest->envp;
//	       while (*(++env))
//	              puts(*env);

	        pthread_t runThreadHandle;			// The thread that runs the shell
	        RunShellArgs *pRunShellArgs;		// The arguments to that thread

	        IpcPipe *serverconn;				// Connection to the server shell process

	        if(id.empty()) {
	        	// Called for first time on a shell, create a new shell process
	        	// and an id
	        	serverconn = new IpcPipe(getpid(), false);
	        	id = string(serverconn->getPair());
	        	processInfoMap[id] = new ProcessInfo(pRequest);

	        	pRunShellArgs = new RunShellArgs(this, shellPath, id);
	        	::pthread_create(&runThreadHandle, NULL, runShell, (void *)pRunShellArgs);
	        }
	        else {
	        	// Existing shell process
	        	// Validate it's running
	        	if(processInfoMap.count(id)==0 || !processInfoMap[id]->shellIsActive) {
	        		::FCGX_PutS( "Status: 500\r\n\r\nNo active shell\r\n", pRequest->out);
	        		::FCGX_Finish_r(pRequest);

	        		fprintf(stderr, "Error 500 no active shell for %s\n", id.c_str());
	        		continue;
	        	}

	        	// Validate correct sequence
	        	if(reqSeq != processInfoMap[id]->lastSequence+1) {
	        		::FCGX_PutS( "Status: 500\r\n\r\nOut of sequence\r\n", pRequest->out);
	        		::FCGX_Finish_r(pRequest);

	        		fprintf(stderr, "Error 500 out of sequence for %s\n", id.c_str());
	        		continue;
	        	}

	        	processInfoMap[id]->lastSequence = reqSeq;

	        	// Make a connection and send it a message
	        	serverconn = new IpcPipe(id.c_str(), false);
	        	processInfoMap[id]->activeRequest = pRequest;

	        	serverconn->send(UserMessage::toSerialized(userMessage).c_str());
	        }

        	processInfoMap[id]->threadCompleteOccurred = false;

	        // Create a thread to handle the inbound message
	        ReadIncomingArgs *pReadIncomingArgs = new ReadIncomingArgs(this, serverconn);
	        pthread_t incomingThreadHandle;
	        ::pthread_create(&incomingThreadHandle, NULL, readIncoming, (void *)pReadIncomingArgs);
	        processInfoMap[id]->incomingThreadHandle = incomingThreadHandle;


	        fprintf(stderr, "Request processing passed to threads\n");

	    }

	    // restore stdio streambufs
	    cin.rdbuf(cin_streambuf);
	    cout.rdbuf(cout_streambuf);

	}

private:
	/**
	 * Note this is not thread safe due to the static allocation of the
	 * content_buffer.
	 */
	 string get_request_content(const FCGX_Request & request) {
	    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
	    unsigned long content_length = STDIN_MAX;

	    if (content_length_str) {
	        content_length = strtol(content_length_str, &content_length_str, 10);
	        if (*content_length_str) {
	            cerr << "Can't Parse 'CONTENT_LENGTH='"
	                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
	                 << "'. Consuming stdin up to " << STDIN_MAX << endl;
	        }

	        if (content_length > STDIN_MAX) {
	            content_length = STDIN_MAX;
	        }
	    } else {
	        // Do not read from stdin if CONTENT_LENGTH is missing
	        content_length = 0;
	    }

	    char * content_buffer = new char[content_length];
	    cin.read(content_buffer, content_length);
	    content_length = cin.gcount();

	    // Chew up any remaining stdin - this shouldn't be necessary
	    // but is because mod_fastcgi doesn't handle it correctly.

	    // ignore() doesn't set the eof bit in some versions of glibc++
	    // so use gcount() instead of eof()...
	    do cin.ignore(1024); while (cin.gcount() == 1024);

	    string content(content_buffer, content_length);
	    delete [] content_buffer;
	    return content;
	}

	static void sigUsr1Handler(int sig) {
		fprintf(stderr, "Got signal %d\n", sig);
	}

	static void * readIncoming(void *pVoidArgs) {
		ReadIncomingArgs *pArgs = (ReadIncomingArgs *)pVoidArgs;

		string id = pArgs->pipe->getPair();

		string content;
		while(!pArgs->pThis->processInfoMap[id]->finishIncoming && content.empty()) {
			 content = pArgs->pipe->receive();

			if(content.empty()) {
				fprintf(stderr, "readIncoming got empty message\n");
			}
			else {
				fprintf(stderr, "readIncoming got message %s\n", content.c_str());
			}
		}


		FCGX_PutS( "Content-type: text/html\r\n\r\n",
				pArgs->pThis->processInfoMap[id]->getOutStream() );
		FCGX_PutS(content.c_str(),
				pArgs->pThis->processInfoMap[id]->getOutStream() );
		FCGX_FFlush(pArgs->pThis->processInfoMap[id]->getOutStream() );

		// Finish the request
		FCGX_Finish_r(pArgs->pThis->processInfoMap[id]->activeRequest);

		// remove our details
		delete pArgs->pThis->processInfoMap[id]->activeRequest;
		pArgs->pThis->processInfoMap[id]->activeRequest = NULL;
		pArgs->pThis->processInfoMap[id]->incomingThreadHandle = NULL;

		fprintf(stderr, "readIncoming notifying completion\n");
		boost::lock_guard<boost::mutex> lock(pArgs->pThis->processInfoMap[id]->threadCompleteLockMutex);
		fprintf(stderr, "readIncoming has lock\n");
		pArgs->pThis->processInfoMap[id]->threadCompleteOccurred = true;
		pArgs->pThis->processInfoMap[id]->threadCompleteCond.notify_one();
		pArgs->pThis->processInfoMap[id]->incomingThreadHandle = NULL;

		delete pArgs;

		return NULL;
	}

	static void* runShell(void *pVoidArgs) {
		RunShellArgs *pArgs = (RunShellArgs *)pVoidArgs;

		setenv("HTMLTAIL_ID", pArgs->id.c_str(), true);
		boost::filesystem::path scriptPath(pArgs->path);
		setenv("HTMLTAIL_SCRIPTPATH", scriptPath.leaf().c_str(), true );

		pid_t spawnedPid = ::fork();
		if(spawnedPid==0) {
			::execl("/bin/sh", "-c", pArgs->path.c_str(), (char *)NULL);

			perror("error in execl");
		}
		else if (spawnedPid < 0) {
			perror("error forking");
		}
		else {
			pArgs->pThis->processInfoMap[pArgs->id]->shellIsActive = true;

			fprintf(stderr, "Waiting for pid %d\n", spawnedPid);

			int status;
			::waitpid(spawnedPid, &status, 0);

			fprintf(stderr, "Wait for %d finished with %d\n", spawnedPid, status);
			pArgs->pThis->processInfoMap[pArgs->id]->shellIsActive = false;

//			sleep(2);	// let the last message come through
		}



		// wait for message processing
		{
			boost::unique_lock<boost::mutex> lock(pArgs->pThis->processInfoMap[pArgs->id]->threadCompleteLockMutex);
			fprintf(stderr, "runShell has lock\n");
			if(!pArgs->pThis->processInfoMap[pArgs->id]->threadCompleteOccurred) {
				boost::system_time const timeout=boost::get_system_time() + boost::posix_time::milliseconds(2000);

				pArgs->pThis->processInfoMap[pArgs->id]->threadCompleteCond.timed_wait(lock, timeout);
			}
		}

		// terminate any live message processor
		fprintf(stderr, "Signal incomingThread\n");
		pArgs->pThis->processInfoMap[pArgs->id]->finishIncoming = true;

		if(pArgs->pThis->processInfoMap[pArgs->id]->incomingThreadHandle != NULL && ::pthread_kill(pArgs->pThis->processInfoMap[pArgs->id]->incomingThreadHandle, SIGUSR1) != ESRCH) {
			fprintf(stderr, "Waiting for incomingThread termination\n");
			void * incomingRetVal;
			::pthread_join(pArgs->pThis->processInfoMap[pArgs->id]->incomingThreadHandle, &incomingRetVal);
		}
		else {
			fprintf(stderr, "runShell thread %s terminating after request terminated by message\n", pArgs->id.c_str());
		}

		fprintf(stderr, "runShell thread exits\n");

		// Remove the entry
		delete pArgs->pThis->processInfoMap[pArgs->id];
		pArgs->pThis->processInfoMap.erase(pArgs->id);

		delete pArgs;

		return NULL;
	}

	string getCookieId(FCGX_Request request) {
		string cookieJar = FCGX_GetParam("HTTP_COOKIE", request.envp);

		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(";");

		tokenizer cookies(cookieJar, sep);

		string cname("webtail_id=");
		for (tokenizer::iterator cook_iter = cookies.begin();cook_iter != cookies.end(); ++cook_iter) {
			string s(*cook_iter);
			if(s.compare(0, cname.length(), cname) == 0) {
				return s.substr(cname.length()+1);
			}
		}

		return "";
	}

	// Maximum bytes
	static const unsigned long STDIN_MAX = 1000000;

	map <string, ProcessInfo *> processInfoMap;

//	map <string, EventInfo *> incomingThreadCompleteEvents;
//	map <string, FCGX_Request *> activeRequestMap;
//	map <string,pthread_t > incomingThreadHandleMap;
//	set <string> activeShellThreads;
};


#endif /* FCGISERVER_H_ */
