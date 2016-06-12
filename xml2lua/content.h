#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <memory>
#include <functional>

using namespace std;

class Content {
private:
	std::shared_ptr<map<string, std::shared_ptr<vector<string>>>> _pMapCntr;
	map<string, int> _mapMark;
	map<string, int> _mapTitle;
	vector<string> _vecTitle;
public:
	Content() :_pMapCntr(new map<string, std::shared_ptr<vector<string>>>()) {};
	Content(std::ifstream& ifs);

	void open(std::ifstream& ifs);
	void open(const string& name);
	bool hasKey(const string& key) { return _pMapCntr->find(key) != _pMapCntr->end(); };
	void writeTo(const string& path);
	void writeTo(std::ofstream& ofs);
	void addTitle(const string& title);
	bool hasTitle(const string& title) { return _mapTitle.find(title) != _mapTitle.end(); };
	const vector<string>& getTitles() const;
	std::shared_ptr<vector<string>> get(const string& key);
	std::shared_ptr<vector<string>> createPair(const string& key);
	void mark(const string& key) { _mapMark.emplace(key, 1); };
	bool hasMarked(const string& key) { return _mapMark.find(key) != _mapMark.end(); }
	void setProto(const string& key, const string& proto);
	void setValue(const string&key, const string& title, const string& value);

	const std::shared_ptr<map<string, std::shared_ptr<vector<string>>>> getContent();
	const map<string, int>& getTitleColMap() { return _mapTitle; };
}; 
