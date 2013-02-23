
#ifndef IPC_H
#define IPC_H

#include <string>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <boost/lexical_cast.hpp>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

using namespace std;

class Ipc {
public:
	Ipc(int pid) {
		string guid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

		char pairbuf[128];
		snprintf(pairbuf, 127, "webtail_%d_%s", pid, guid.c_str());
		pair = string(pairbuf);

		data_fd = -1;
	}

	Ipc(const char *pair) {
		this->pair = pair;
		data_fd = -1;
	}

	const char *getPair() {
		return pair.c_str();
	}

	void send(const char *message) {
		if(!connect()) {
			return;
		}

		uint nlen = htonl(strlen(message));
		int written = write(data_fd, &nlen, 4);
		if(written < 4) {
			perror("write error");
			return;
		}

		uint n=0;
		while(n<strlen(message)) {
			int written = write(data_fd, &message[n], strlen(message)-n);
			if(written < 0) {
				perror("write error");
				return;
			}

			n += written;
		}
	}

	string receive() {
		if(!connect()) {
			return "";
		}

		uint nlen;
		if(read(data_fd,&nlen,4) < 4) {
			perror("read buffer length");
			return "";
		}

		uint len = ntohl(nlen);

		int rc;
		char buf[len+1];
		uint n=0;
	    while ( n<len && (rc=read(data_fd, &buf[n], len-n)) > 0) {
	    	n += rc;
	    }

	    if (rc == -1) {
			perror("read");
			return "";
	    }

	    buf[len] = 0;
	    return string(buf);
	}

	static bool testme() {
		Ipc conn(::getpid());
		boost::thread echo_thread(Ipc::echoer, conn.getPair());

		conn.send("Hello!");

		sleep(2);

		conn.send("stop");

		echo_thread.join();

		return true;
	}

private:
	static void echoer(const char *id) {
		Ipc conn(id);

		string msg;
		do {
			msg = conn.receive();
			if(msg.empty()) {
				break;
			}

			string echomsg = "ECHO:" + string(msg);

			conn.send(echomsg.c_str());

		} while(strcmp(msg.c_str(), "stop") != 0);
	}

	bool connect() {
		if(data_fd > 0) {
			return true;
		}

		static int socket_fd;

		while(true) {
			sleep(5);
			if ( (socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
				perror("socket error");
				return false;
			}

			// First try and connect
			struct sockaddr_un addr;
			memset(&addr, 0, sizeof(addr));
			addr.sun_family = AF_UNIX;
			strncpy(addr.sun_path, pair.c_str(), sizeof(addr.sun_path)-1);

			if (::connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) >= 0) {
			  data_fd = socket_fd;

			  cout << "Thread: " << boost::this_thread::get_id() << " connected";

			  return true;	// connected
			}

			if(errno != ECONNREFUSED && errno != ENOENT) {
				perror("connect error");
				return false;
			}

			// If otherside isn't listening, listen ourselves
			close(socket_fd);
			if ( (socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
				perror("socket error");
				return false;
			}

			// unlink(pair.c_str());

			if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
				if(errno==EADDRINUSE) {
					continue;	// Something already binded
				}

			    perror("bind error");
			    return false;
			  }

			if (listen(socket_fd, 5) == -1) {
				if(errno==EADDRINUSE || errno==EINVAL) {
					close(socket_fd);
					continue;	// Something already listened
				}

				perror("listen error");
				return false;
			}

			cout << "Thread: " << boost::this_thread::get_id() << " in accept";
			int connect_fd;
			if ( (connect_fd = accept(socket_fd, NULL, NULL)) == -1) {
			  perror("accept error");

			  close(socket_fd);
			  continue;
			}

			cout << "Thread: " << boost::this_thread::get_id() << " accept ok";

			data_fd = connect_fd;
			return true;	// connected
		}

		return false;
	}

	int data_fd;
	string pair;
};


#endif
