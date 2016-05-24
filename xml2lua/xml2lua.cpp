
#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <fstream>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;


void walk(string& buf, const XMLElement* ele, int level) {
	int base_wp_num = level * 4;
	string base_wp = string(base_wp_num + 4, ' ');
	string base_wp_back4 = string(base_wp_num, ' ');
	buf += base_wp_back4 + "{\n";

	boost::format fmt("%s%s=\"%s\",\n");
	buf += (fmt % base_wp % "__cname" % ele->Name()).str();

	for (const XMLAttribute* attr = ele->FirstAttribute(); attr != nullptr; attr = attr->Next()) {
		buf += (fmt % base_wp % attr->Name() % attr->Value()).str();
	}
	const XMLElement* cele = ele->FirstChildElement();
	if (cele) {
		buf += base_wp + "children = {\n";
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
			buf += (fmt % base_wp % "children" % text).str();
	}

	buf += base_wp_back4 + "}";
}


int main(int argc, char** argv) {
	if (argc != 3)
		cout << "Usage : in_filename out_filename" << endl;

	char* in_name = argv[1];
	char* out_name = argv[2];
	
	XMLDocument* document = new XMLDocument();
	document->LoadFile(in_name);

	XMLElement* root = document->RootElement();
	
	string buf;
	walk(buf, root, 0);

	ofstream* fsout = new ofstream(out_name);
	fsout->write(buf.c_str(), buf.size());
	fsout->flush();
	fsout->close();

	cout << "completed" << endl;
	getchar();

	return 0;
}