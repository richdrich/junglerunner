
#ifndef IPC_H
#define IPC_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

using namespace std;

class IpcPipe {
public:
	IpcPipe(int pid, bool inbound) {
		string guid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

		char pairbuf[128];
		snprintf(pairbuf, 127, "webtail_%d_%s", pid, guid.c_str());
		pair = string(pairbuf);

		this->inbound = inbound;
		connected[0] = false;
		connected[1] = false;
	}

	IpcPipe(const char *pair, bool inbound) {
		this->pair = pair;
		this->inbound = inbound;
		connected[0] = false;
		connected[1] = false;
	}

	const char *getPair() {
		return pair.c_str();
	}

	void send(const char *message) {
		if(!connect(false)) {
			return;
		}

		// printf("Write, inbound=%s\n", inbound ? "true" : "false");

		uint nlen = htonl(strlen(message));
		int written = fwrite(&nlen, 1, 4, pipes[0]);
		if(written < 4) {
			perror("write error");
			return;
		}

		uint n=0;
		while(n<strlen(message)) {
			int written = fwrite(&message[n], 1, strlen(message)-n,  pipes[0]);
			if(written < 0) {
				perror("fwrite error");
				return;
			}

			n += written;
		}

		fflush(pipes[0]);
	}

	string receive() {
		if(!connect(true)) {
			return "";
		}

		// printf("Read, inbound=%s\n", inbound ? "true" : "false");
		uint nlen;
		uint readlen;
		if((readlen = fread(&nlen, 1, 4, pipes[1])) < 4) {
			if(readlen ==0) {
				fclose(pipes[1]);
				connected[1] = false;
				fprintf(stderr, "fread length disconnected\n");
				return "";
			}

			perror("fread buffer length");
			return "";
		}

		uint len = ntohl(nlen);
		// printf("Got %d, inbound=%s\n", len, inbound ? "true" : "false");

		int rc=0;
		char buf[len+1];
		uint n=0;
	    while ( n<len && (rc=fread(&buf[n], 1, len-n, pipes[1])) > 0) {
	    	n += rc;
	    }

		if(rc ==0 && n<len) {
			fclose(pipes[1]);
			connected[1] = false;
			// fprintf(stderr, "fread buffer disconnected\n");
			return "";
		}

	    if (rc == -1) {
			perror("fread");
			return "";
	    }

	    buf[len] = 0;
	    return string(buf);
	}

	static bool testme() {
		IpcPipe conn(::getpid(), false);
		boost::thread echo_thread(IpcPipe::echoer, conn.getPair());

		conn.send("Hello!");

		sleep(2);

		conn.send("stop");

		echo_thread.join();

		return true;
	}

private:
	static void echoer(const char *id) {
		IpcPipe conn(id, true);

		string msg;
		do {
			msg = conn.receive();
			if(msg.empty()) {
				break;
			}

			printf("Received by echoer: %s\n", msg.c_str());
			string echomsg = "ECHO:" + string(msg);

			conn.send(echomsg.c_str());

		} while(strcmp(msg.c_str(), "stop") != 0);
	}

	bool connect(bool forRead) {

		int pidx = forRead ? 1 : 0;
		if(connected[pidx]) {
			return true;
		}

		string pipedir("/var/run/htmltail");
		boost::filesystem::create_directories(pipedir);

		// If inbound (the shell command) and writing, use the _in pipe
		// If inbound (the shell command) and reading, use the _out pipe
		// If outbound (the server) and writing, use the _out pipe
		// If outbound (the server) and reading, use the _in pipe
		string pipefile = pipedir + string("/") + pair + ((inbound == forRead) ? "_out" : "_in");
		const char *dir = forRead ? "r" : "w";

		if(access(pipefile.c_str(), F_OK)!=0) {
			int status = ::mkfifo(pipefile.c_str(), S_IWUSR | S_IRUSR);
			if(status < 0 && errno != EEXIST) {
				perror("mkfifo error");
				return false;
			}
		}

		//fprintf(stderr, "(%s) Opening pipe %s for %s\n",
		//		inbound ? "inbound" : "outbound", pipefile.c_str(), dir);
		pipes[pidx] = fopen(pipefile.c_str(), dir);
		if(pipes[pidx]==NULL) {
			perror("fopen");
			return false;
		}
		//fprintf(stderr, "Pipe %s connected\n", pipefile.c_str());

		//fprintf(stderr, "Connected, inbound=%s\n", inbound ? "true" : "false");

		connected[pidx] = true;
		return true;
	}

	bool connected[2];
	FILE * pipes[2];
	string pair;
	bool inbound;
};


#endif
