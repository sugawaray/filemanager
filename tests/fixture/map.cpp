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

} // fixture
} // test
} // ml

