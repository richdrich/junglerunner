/*
 * processinfo.h
 *
 *  Created on: Feb 23, 2013
 *      Author: richardparratt
 */

#ifndef PROCESSINFO_H_
#define PROCESSINFO_H_

#include "fcgio.h"
#include <boost/thread.hpp>

class ProcessInfo {
public:
	ProcessInfo(::FCGX_Request *request) {
		activeRequest = request;
		shellIsActive = false;
		threadCompleteOccurred = false;
		lastSequence = 0;
		finishIncoming = false;
	}

	FCGX_Stream * getOutStream() {
		return activeRequest->out;
	}

	boost::condition_variable threadCompleteCond;
	boost::mutex threadCompleteLockMutex;
	bool threadCompleteOccurred;
	FCGX_Request *activeRequest;
	pthread_t incomingThreadHandle;
	bool shellIsActive;
	int lastSequence;
	bool finishIncoming;
};



#endif /* PROCESSINFO_H_ */
