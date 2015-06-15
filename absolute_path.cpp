#include <sstream>
#include <stdexcept>
#include "absolute_path.h"
#include "string_utils.h"
#include "utils_assert.h"

namespace fs {

using std::any_of;
using std::back_inserter;
using std::copy;
using std::find;
using std::invalid_argument;
using std::logic_error;
using std::ostream_iterator;
using std::ostringstream;
using std::runtime_error;
using std::string;
using su::split_string;

class Absolute_path::Is_invalid {
public:
	bool operator()(const string& op) const throw() {
		return op == "." || op == "..";
	}
};

Absolute_path::Absolute_path()
{
}

Absolute_path::Absolute_path(const string& stringrep)
{
	init(stringrep);
}

Absolute_path::Absolute_path(const char *stringrep)
{
	init(stringrep);
}

Absolute_path::Absolute_path(CIter first, CIter last)
{
	copy(first, last, back_inserter(comps));
}

void Absolute_path::init(const string& stringrep)
{
	if (stringrep.empty() || stringrep.at(0) != '/')
		throw invalid_argument("fs::Absolute_path");
	split_string(stringrep, "/", back_inserter(comps));
	if (comps.back() == "")
		comps.pop_back();
	Assert<logic_error>(!comps.empty());
	if (find(comps.begin() + 1, comps.end(), "") != comps.end())
		throw invalid_argument("fs::Absolute_path");
	if (any_of(comps.begin(), comps.end(), Is_invalid()))
		throw invalid_argument("fs::Absolute_path");
}

string Absolute_path::to_string() const throw()
{
	throw_when_invalid();
	ostringstream os;
	auto it(ostream_iterator<string>(os, "/"));
	copy(comps.begin(), comps.end(), it);
	return os.str();
}

string Absolute_path::to_filepath_string() const throw()
{
	throw_when_invalid();
	if (comps.size() == 1) {
		return "/";
	} else {
		string tmp(to_string());
		return tmp.substr(0, tmp.length() - 1);
	}
}

Absolute_path Absolute_path::parent() const throw(runtime_error, Invalid_path)
{
	throw_when_invalid();
	if (comps.size() == 1)
		throw Invalid_path();
	return Absolute_path(comps.begin(), comps.end() - 1);
}

Absolute_path Absolute_path::child(const string& component) const
{
	if (component.empty())
		throw invalid_argument("fs::Absolute_path");
	if (component.find_first_of('/') != string::npos)
		throw invalid_argument("fs::Absolute_path");
	if (component == "..")
		throw invalid_argument("fs::Absolute_path");
	if (component == ".")
		throw invalid_argument("fs::Absolute_path");
	throw_when_invalid();
	Absolute_path tmp(*this);
	tmp.comps.push_back(component);
	return tmp;
}

string Absolute_path::basename() const throw(runtime_error)
{
	throw_when_invalid();
	return comps.back();
}

} // fs

