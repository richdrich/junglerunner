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
	ParamEntry(boost::variant<string, int> value, bool optional, string section) {
		this->values.push_back(value);
		this->optional = optional;
		this->section = section;
	}

	ParamEntry(const ParamEntry &other) {
		this->values = other.values;
		this->optional = other.optional;
		this->section = other.section;
	}

	ParamEntry() {
		optional = false;
	}

	void add(vector< boost::variant<string, int> > other) {
		values.insert(values.end(), other.begin(), other.end());
	}

	vector< boost::variant<string, int> > values;
	bool optional;
	string section;
};


#endif /* PARAMENTRY_H_ */
