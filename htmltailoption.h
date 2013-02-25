/*
 * htmltailoption.h
 *
 *  Created on: Feb 13, 2013
 *      Author: richardparratt
 */

#ifndef HTMLTAILOPTION_H_
#define HTMLTAILOPTION_H_

#include <iostream>
#include <fstream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <ctemplate/template.h>

#include "ipcpipe.h"
#include "ctemps/alltemplates.h"
#include "usermessage.h"

using namespace std;
using namespace boost;

struct to_string_visitor : boost::static_visitor<>
{
  std::string str;

  template <typename T>
  void operator()(T const& item)
  {
    str = boost::lexical_cast<std::string>(item);
  }

};

class HtmltailOption {
public:
	HtmltailOption() {
		for(int n=0; n<NumTemplates; n++) {
			allTemplateMap[string(AllTemplates[n][0])] = AllTemplates[n][1];
		}

		const char *script = ::getenv("HTMLTAIL_SCRIPTPATH");
		parameters["SCRIPT"] = string(script==NULL ? "" : script);
		const char * idc = ::getenv("HTMLTAIL_ID");
		id = string(idc==NULL ? "" : idc);
		parameters["ID"]  = id;
		parameters["SEQ"] = readSequence();
	}

	virtual ~HtmltailOption() {};

	void setOptionName(string optionName) {
		this->optionName = optionName;
	}

	void addParameters(map<string, variant<string, int> > addPars) {
		parameters.insert(addPars.begin(), addPars.end());
	}

	virtual int run() {

		string page = pageFromTemplate(getTemplateText().c_str());

		getPipe()->send(page.c_str());

		string response = getPipe()->receive();
		UserMessage msg = UserMessage::fromSerialized(response.c_str());

		// fprintf(stderr, "setenv sequence %d\n", msg.sequence);
		writeSequence(boost::lexical_cast<string>(msg.sequence));

		return processResultMsg(msg);
	}

protected:
	virtual int processResultMsg(UserMessage msg) {
		if(msg.args.count("cancel")) {
			return -1;
		}

		return 0;
	}

	shared_ptr<IpcPipe> getPipe() {

		const char *id = ::getenv("HTMLTAIL_ID");
		shared_ptr<IpcPipe> res(new IpcPipe(id, true));

		return res;
	}

	string getTemplateText() {
		return allTemplateMap[optionName];
	}

	string pageFromTemplate(const char *tmplText) {
		ctemplate::TemplateDictionary dict("page");

		for (std::map<string, variant<string, int> >::iterator iter = parameters.begin();
				iter != parameters.end(); iter++) {
			to_string_visitor vis;
			apply_visitor(vis, iter->second);
			dict.SetValue(iter->first, vis.str);
		}

		string res;
		ctemplate::StringToTemplateCache("internal.htt", tmplText, ctemplate::DO_NOT_STRIP);
		ctemplate::ExpandTemplate("internal.htt", ctemplate::DO_NOT_STRIP, &dict, &res);
		return res;
	}

	string readSequence() {
		string pipedir("/var/run/htmltail");

		string seqFname = pipedir + string("/") + id + "_seq";
		ifstream seqStream(seqFname.c_str());

		if(!seqStream) {
			return string("1");
		}

		string seq;
		seqStream >> seq;
		seqStream.close();

		return seq;
	}

	void writeSequence(string seq) {
		string pipedir("/var/run/htmltail");

		string seqFname = pipedir + string("/") + id + "_seq";

		ofstream seqStream(seqFname.c_str());
		seqStream << seq;
		seqStream.close();
	}

	string optionName;
	string id;
	map<string, variant<string, int> > parameters;
	map<string, string> allTemplateMap;
};


#endif /* HTMLTAILOPTION_H_ */
