/*
 * optiondef.h
 *
 *  Created on: Feb 26, 2013
 *      Author: richardparratt
 */

#ifndef OPTIONDEF_H_
#define OPTIONDEF_H_

#include <string>
#include <boost/variant.hpp>

using namespace std;

class OptionDef {
public:
	OptionDef(const char *name, bool hasText, boost::variant<string, bool> defaultValue) {
		this->name = string(name);
		this->hasText = hasText;
		this->defaultValue = defaultValue;
	}

	string name;
	bool hasText;
	boost::variant<string, bool> defaultValue;
};


#endif /* OPTIONDEF_H_ */
