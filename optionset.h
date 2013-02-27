/*
 * optionset.h
 *
 *  Created on: Feb 27, 2013
 *      Author: richardparratt
 */

#ifndef OPTIONSET_H_
#define OPTIONSET_H_

#include <string>
#include <boost/variant.hpp>

using namespace std;

class OptionSet {
public:
	OptionSet() {
		defaulted = false;
	}

	OptionSet(boost::variant<bool, string> value, bool defaulted) {
		this->value = value;
		this->defaulted = defaulted;
	}

	boost::variant<bool, string> value;
	bool defaulted;
};


#endif /* OPTIONSET_H_ */
