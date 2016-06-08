
#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include "tinyxml2.h"
#include "content.h"

using namespace std;
using namespace tinyxml2;

boost::format strFmt("%s%s=[[%s]],\n");
boost::format notStrFmt("%s%s=%s,\n");
boost::format keyFmt("%s_%s_%d");
const vector<string> VEC_PREFIX = { "str_" };

string filename_no_ext;
int maxId = 1;
Content content;

inline string get_file_name_no_ext(const string filename) {
	int last = filename.find_last_of(".");
	return filename.substr(0, last - 1);
}

inline string get_file_name_ext(const string filename) {
	int last = filename.find_last_of(".");
	return filename.substr(last + 1);
}

inline bool isStr(const string& name) {
	for (auto prefix : VEC_PREFIX) {
		if (boost::starts_with(name, prefix))
			return true;
	}

	return false;
}

string parse(string name, const vector<string>& vecPrefix, const vector<string>& vecSuffix, string& lang) {
	for (auto prefix : vecPrefix) {
		if (boost::starts_with(name, prefix)){
			name = name.substr(prefix.size());
			break;
		}
	}

	for (auto suffix : vecSuffix) {
		if (boost::ends_with(name, suffix)) {
			int index = name.size() - suffix.size();
			lang = name.substr(index + 1);
			name = name.substr(0, name.size() - suffix.size());
			break;
		}
	}

	return name;
}

void toSuffix(vector<string>& vecSuffix){
	for (auto &value : vecSuffix) {
		if (!boost::starts_with(value, "_"))
			value = "_" + value;
	}
}

void walk(string& buf, const XMLElement* ele, int level) {
	int base_wp_num = level * 4;
	string base_wp = string(base_wp_num + 4, ' ');
	string base_wp_back4 = string(base_wp_num, ' ');
	buf += base_wp_back4 + "{\n";

	buf += (strFmt % base_wp % "__cname" % ele->Name()).str();

	for (const XMLAttribute* attr = ele->FirstAttribute(); attr != nullptr; attr = attr->Next()) {
		string name = attr->Name();
		boost::format* pfmt = &notStrFmt;

		if (isStr(name)) {
			pfmt = &strFmt;

			vector<string> vecSuffix = content.getTitles();
			toSuffix(vecSuffix);
			name = parse(name, VEC_PREFIX, vecSuffix);
			string key = (keyFmt % filename_no_ext % name % maxId).str();
			if (content.hasKey(key))
			{

			}
		}
		buf += (*pfmt % base_wp % name % attr->Value()).str();
	}
	const XMLElement* cele = ele->FirstChildElement();
	if (cele) {
		buf += base_wp + "__children = {\n";
		for (; cele != nullptr; cele = cele->NextSiblingElement()) {
			walk(buf, cele, level + 2);
			buf += ",\n";
		}
		buf += base_wp + "},\n";
	}
	else
	{
		const char* text = ele->GetText();
		if (text)
			buf += (strFmt % base_wp % "__children" % text).str();
	}

	buf += base_wp_back4 + "}";
	maxId++;
}


int main(int argc, char** argv) {
	if (argc != 3) {
		cout << "Usage : in_filename out_filename" << endl;
		return 0;
	}

	string in_name = argv[1];

	filename_no_ext = get_file_name_no_ext(in_name);
	string outLua = filename_no_ext + ".lua";
	string outKey = filename_no_ext + ".key";
	
	XMLDocument* document = new XMLDocument();
	document->LoadFile(in_name.c_str());
	XMLElement* root = document->RootElement();

	content.open(outKey);
	
	string buf;
	walk(buf, root, 0);

	content.writeTo(outKey);

	ofstream* fsout = new ofstream(outLua);
	fsout->write(buf.c_str(), buf.size());
	fsout->flush();
	fsout->close();

	cout << "completed" << endl;

	getchar();

	return 0;
}
