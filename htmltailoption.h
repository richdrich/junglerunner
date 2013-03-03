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
#include <boost/algorithm/string.hpp>
#include <ctemplate/template.h>

#include "ipcpipe.h"
#include "ctemps/alltemplates.h"
#include "usermessage.h"
#include "paramentry.h"
#include "optionset.h"
#include "debug.h"

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
		parameters["SCRIPT"] = ParamEntry(string(script==NULL ? "" : script), false, "");
		const char * idc = ::getenv("HTMLTAIL_ID");
		id = string(idc==NULL ? "" : idc);
		parameters["ID"]  = ParamEntry(id, false, "");
		parameters["SEQ"] = ParamEntry(readSequence(), false, "");
	}

	virtual ~HtmltailOption() {};

	void setCommandName(string optionName) {
		this->optionName = optionName;
	}

	void addParameters(map<string, ParamEntry > addPars) {
		parameters.insert(addPars.begin(), addPars.end());
	}

	void setOptions(map<string, OptionSet> options) {
		this->options = options;
	}

	virtual int run() {

		string page = pageFromTemplate(getTemplateText().c_str());

		getPipe()->send(page.c_str());

		string response = getPipe()->receive();
		UserMessage msg = UserMessage::fromSerialized(response.c_str());

		// debugf( "setenv sequence %d\n", msg.sequence);
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

		map <string, vector<ctemplate::TemplateDictionary *> > sectionDicts;

		for (std::map<string, ParamEntry >::iterator iter = parameters.begin();
				iter != parameters.end(); iter++) {

			if(!iter->second.section.empty()) {

				for(uint rowIdx=0; rowIdx < iter->second.values.size(); rowIdx++) {
					boost::variant<string, int> rowItem = iter->second.values[rowIdx];

					to_string_visitor vis;
					apply_visitor(vis, rowItem);

					debugf( "Set in section %s[%d] %s = %s\n",
						iter->second.section.c_str(), rowIdx, iter->first.c_str(), vis.str.c_str());

					ctemplate::TemplateDictionary* sub_dict;
					if(sectionDicts.count(iter->second.section)) {
						if(sectionDicts[iter->second.section].size() > rowIdx) {
							sub_dict = sectionDicts[iter->second.section][rowIdx];
						}
						else {
							sub_dict = dict.AddSectionDictionary(iter->second.section);
							sectionDicts[iter->second.section].push_back(sub_dict);
						}
					}
					else {
						 sub_dict = dict.AddSectionDictionary(iter->second.section);
						 sectionDicts[iter->second.section] = vector<ctemplate::TemplateDictionary *>();
						 sectionDicts[iter->second.section].push_back(sub_dict);
					}

					sub_dict->SetValue(iter->first, vis.str);
				}
			}
			else {
				to_string_visitor vis;
				apply_visitor(vis, iter->second.values[0]);

				dict.SetValue(iter->first, vis.str);
			}
		}

		for (std::map<string, OptionSet >::iterator iter = options.begin();
						iter != options.end(); iter++) {
			to_string_visitor vis;
			apply_visitor(vis, iter->second.value);

			string sectionName = boost::replace_all_copy(to_upper_copy(iter->first.substr(2)), "-", "_");

			// debugf( "Set option %s : %s\n", sectionName.c_str(), vis.str.c_str());
			dict.SetValueAndShowSection(sectionName, vis.str, string("IF_") + sectionName);
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
	map<string, ParamEntry> parameters;
	map<string, OptionSet> options;
	map<string, string> allTemplateMap;
};


#endif /* HTMLTAILOPTION_H_ */
