/*
 * paramentry.h
 *
 *  Created on: Feb 25, 2013
 *      Author: richardparratt
 */

#ifndef PARAMENTRY_H_
#define PARAMENTRY_H_

#include <string>
#include <boost/variant.hpp>

using namespace std;

struct ParamEntry {
	ParamEntry(boost::variant<string, int> value, bool optional=false) {
		this->value = value;
		this->optional = optional;
	}

	ParamEntry(const ParamEntry &other) {
		this->value = other.value;
		this->optional = other.optional;
	}

	ParamEntry() {
		optional = false;
	}

	boost::variant<string, int> value;
	bool optional;
};


#endif /* PARAMENTRY_H_ */
