#include <algorithm>
#include <deque>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include "except.h"
#include "path.h"
#include "string_utils.h"
#include "utils_assert.h"

namespace fs {

using std::back_inserter;
using std::copy;
using std::deque;
using std::logic_error;
using std::invalid_argument;
using std::ostream_iterator;
using std::ostringstream;
using std::remove;
using std::string;
using fs::Invalid_path;
using su::split_string;

inline void Path::validate()
{
	Assert<logic_error>(!seq.empty());
	Assert<logic_error>(!s.empty());
	Assert<logic_error>(*s.rbegin() == '/');
}

Path::Path(const string& stringrep)
{
	if (stringrep == "")
		throw invalid_argument("Path");
	split_string(stringrep, "/", back_inserter(seq));
	seq.erase(remove(seq.begin() + 1, seq.end(), ""), seq.end());
	ostringstream os;
	copy(seq.begin(), seq.end(), ostream_iterator<string>(os, "/"));
	s = os.str();
	validate();
}

bool Path::is_absolute() const throw()
{
	if (seq.front() == "")
		return true;
	else
		return false;
}

string Path::stringrep() const throw()
{
	return s;
}

Absolute_path Path::to_absolute_path(const Absolute_path& current_dir) const
{
	deque<string> tmp;
	if (!is_absolute())
		current_dir.get_components(back_inserter(tmp));
	copy(seq.begin(), seq.end(), back_inserter(tmp));
	deque<string> deq;
	for (auto i(tmp.begin()); i != tmp.end(); ++i) {
		if (*i == ".")
			continue;
		else if (*i == ".." && deq.size() <= 1)
			throw Invalid_path();
		else if (*i == "..")
			deq.pop_back();
		else
			deq.push_back(*i);
	}
	ostringstream os;
	auto it(ostream_iterator<string>(os, "/"));
	copy(deq.begin(), deq.end(), it);
	return Absolute_path(os.str());
}

} // fs
