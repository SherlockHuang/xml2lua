
#include "content.h"
#include "boost\tokenizer.hpp"
#include <iostream>

using namespace boost;

void split(const string& data, const string& sep, vector<string>& container) {
	char_separator<char> char_sep(sep.c_str());
	tokenizer<char_separator<char>> tok(data, char_sep);

	for (const string& item : tok) {
		container.push_back(item);
	}
}

inline void write(std::ofstream& ofs, const string& data) {
	ofs.write(data.c_str(), data.size());
}

Content::Content(ifstream& ifs) : Content() {
	open(ifs);
}

std::shared_ptr<vector<string>> Content::get(const string& key) {
	if (hasKey(key)) {
		return _pMapCntr->find(key)->second;
	}

	return nullptr;
}

void Content::writeTo(ofstream& ofs) {
	string titlePart = "key\tproto";
	write(ofs, titlePart);

	for (const auto& item : _vecTitle) {
		write(ofs, "\t" + item);
	}

	ofs.write("\n", 1);

	for (const auto &pair : *_pMapCntr) {
		const string& key = pair.first;

		if (hasMarked(key)) {
			ofs.write(key.c_str(), key.length());

			for (const string& item : *pair.second) {
				ofs.write("\t", 1);
				ofs.write(item.c_str(), item.length());
			}

			ofs.write("\n", 1);
		}

	}
}

std::shared_ptr<vector<string>> Content::createPair(const string& key) {
	std::shared_ptr<vector<string>> vecValue;
	if (!hasKey(key)) {
		vecValue = std::shared_ptr<vector<string>>(new vector<string>());
		_pMapCntr->emplace(key, vecValue);
	}
	else {
		vecValue = get(key);
	}

	return vecValue;
}

const std::shared_ptr<map<string, std::shared_ptr<vector<string>>>> Content::getContent() {
	return _pMapCntr;
}

void Content::addTitle(const string& title) {
	if (_mapTitle.find(title) == _mapTitle.end()) {
		_mapTitle.emplace(title, _vecTitle.size() + 2);
		_vecTitle.push_back(title);
	}
}

void Content::setProto(const string& key, const string& proto) {
	auto vecContent = get(key);
	if (vecContent->size() == 0) {
		vecContent->push_back(proto);
	}
	else {
		(*vecContent)[0] = proto;
	}

	for (int i = 0; i < _vecTitle.size() + 1; i++) {
		if (vecContent->size() == i) {
			vecContent->push_back(proto);
		}
		else {
			if ((*vecContent)[i] == "") {
				(*vecContent)[i] = proto;
			}
		}
	}
}

void Content::open(const string& path) {
	ifstream ifs(path);
	if (ifs.is_open())
		open(ifs);
}

void Content::open(ifstream& ifs) {
	string line;

	if (getline(ifs, line)) {
		vector<string> data;
		split(line, "\t", data);

		for (int i = 2; i < data.size(); i++) {
			_mapTitle.emplace(data[i], i);
			_vecTitle.push_back(data[i]);
		}
	}

	while (getline(ifs, line)) {
		char_separator<char> sep("\t");
		tokenizer<char_separator<char>> tok(line, sep);
		auto key = *tok.begin();

		if (!hasKey(key)) {
			auto vecValue = std::shared_ptr<vector<string>>(new vector<string>());
			_pMapCntr->emplace(key, vecValue);

			int count = 0;
			for (const string& item : tok) {
				if (count++ > 0) {
					vecValue->push_back(item);
				}
			}
		}
	}
}

void Content::writeTo(const string& path) {
	ofstream ofs(path);
	if (ofs.is_open())
		writeTo(ofs);
}

const vector<string>& Content::getTitles() const {
	return _vecTitle;
}

void Content::setValue(const string& key, const string& title, const string& value) {
	if (hasKey(key) && hasTitle(title)) {
		auto vecValue = get(key);
		auto index = _mapTitle.find(title)->second;

		int delta = index - vecValue->size();
		for (int i = 0; i < delta; i++) {
			vecValue->push_back("");
		}
		
		(*vecValue)[index - 1] = value;

		if (index == 2) {
			setProto(key, value);
		}
	}
}
