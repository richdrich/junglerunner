/*
 * usermessage.h
 *
 *  Created on: Feb 18, 2013
 *      Author: richardparratt
 */

#ifndef USERMESSAGE_H_
#define USERMESSAGE_H_

#include "boost/serialization/map.hpp"
// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace std;


class UserMessage {
public:
	static UserMessage fromSerialized(const char *s) {
		stringstream sstr(s);
		boost::archive::text_iarchive ia(sstr);

		UserMessage newUm;
		ia >> newUm;

		return newUm;
	}

	static string toSerialized(UserMessage &msg) {
		stringstream sstr;
		boost::archive::text_oarchive oa(sstr);

		oa << msg;

		return sstr.str();
	}

    map<string, string> args;
    int sequence;

private:
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & args;
        ar & sequence;
    }

};

BOOST_CLASS_TRACKING(UserMessage, boost::serialization::track_never);


#endif /* USERMESSAGE_H_ */
