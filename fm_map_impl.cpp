#include <cassert>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>
#include "fm.h"
#include "fm_map_impl.h"
#include "receiver_impls.h"
#include "sql_value_spec.h"

namespace fm {

using std::find_if;
using std::logic_error;
using std::min;
using std::string;
using std::vector;
using ml::Map_impl;
using ml::Sql_left_value_spec;
using ml::Receiver_fun;

Fm_map_impl::Fm_map_impl(const string& dbfilepath)
:	Map_impl(dbfilepath)
{
}

int Fm_map_impl::get_file_type(const string& fm_path)
{
	if (fm_path.empty())
		return Type_dir;
	vector<string> values;
	auto inserter(back_inserter(values));
	auto receiver(Receiver_fun(inserter));
	get_values(Sql_left_value_spec(fm_path), receiver);
	if (values.size() == 1 && values.at(0) == fm_path) {
		return Type_file;
	} else if (do_satisfy_dir(fm_path, values)) {
		return Type_dir;
	} else if (values.empty()) {
		if (determine_possibility(fm_path))
			return Not_exist;
		else
			return Impossible;
	} else {
		return Type_file;
	}
}

/* Be carefull. This member function and get_file_type() depend on each other.

   This member function returns true when the given fm path is possible to
   exist or returns false when the given fm path conflicts against current
   mappings. */
bool Fm_map_impl::determine_possibility(const string& fm_path)
{
	string parent(fm_path.substr(0, min(fm_path.length(),
		fm_path.find_last_of('/'))));
	if (parent == fm_path)
		return true;
	int type(get_file_type(parent));
	switch (type) {
	case Type_dir:
	case Not_exist:
		return true;
	case Type_file:
		return false;
	default:
		assert("should not reach." == 0);
		throw logic_error("Fm_map_impl::determine_possibility");
	}
}

namespace {

class Is_descendant {
public:
	Is_descendant(const string& path) : path(path) {
	}

	bool operator()(const string& target) const {
		return target.find(path + "/") == 0;
	}
private:
	const string& path;
};

} // unnamed

bool Fm_map_impl::do_satisfy_dir(const string& fm_path,
	const vector<string>& values)
{
	if (values.empty())
		return false;
	return find_if(values.begin(), values.end(), Is_descendant(fm_path))
		!= values.end();
}

} // fm
