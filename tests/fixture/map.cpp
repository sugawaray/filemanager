#include "map.h"
#include <map.h>
#include <iterator>

namespace ml {
namespace test {
namespace fixture {

using std::string;
using std::vector;

vector<string> get_values(Map& map, const string& category)
{
	vector<string> c;
	c.push_back(category);
	vector<string> r;
	map.get(c, std::back_inserter(r));
	return r;
}

void set_category(Map& map, const string& target, const string& category)
{
	vector<string> c;
	c.push_back(category);
	map.set(target, c.begin(), c.end());
}

void set_category(Map& map, const string& target, const string& category1,
	const string& category2)
{
	vector<string> c;
	c.push_back(category1);
	c.push_back(category2);
	map.set(target, c.begin(), c.end());
}

void set_category(Map& m, const string& t, const string& c1,
	const string& c2, const string& c3)
{
	vector<string> c;
	c.push_back(c1);
	c.push_back(c2);
	c.push_back(c3);
	m.set(t, c.begin(), c.end());
}

} // fixture
} // test
} // ml

