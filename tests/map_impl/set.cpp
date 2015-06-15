#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "set.h"
#include <map_impl.h>
#include <tests/fixture/db.h>
#include <tests/fixture/assert.h>

namespace {

template<class E, class T, class U>
inline void A(E e, T t, U u)
{
	::test::fixture::Assert(e, t, u);
}

} // unnamed

using std::back_inserter;
using std::string;
using std::vector;
using ml::Map_impl;
using ::test::Db_fixture;

typedef Db_fixture F;

START_TEST(should_add_categories)
{
	Db_fixture f;
	vector<string> categories;
	categories.push_back("catA");
	categories.push_back("catB");
	Map_impl map1(f.get_dbfilepath());
	map1.set("target", categories.begin(), categories.end());

	categories.clear();
	Map_impl map2(f.get_dbfilepath());
	vector<string> results;

	categories.push_back("catA");
	map2.get(categories, back_inserter(results));
	fail_unless(results.size() == 1, "the first one");

	categories.at(0) = "catB";
	results.clear();
	map2.get(categories, back_inserter(results));
	fail_unless(results.size() == 1, "the second one");
}
END_TEST

START_TEST(overwrite_categories)
{
	F f;
	Map_impl m(f.get_dbfilepath());
	vector<string> c;
	c.push_back("catA");
	c.push_back("catA1");
	c.push_back("catA2");
	m.set("target", c.begin(), c.end());

	c.clear();
	c.push_back("catB");
	c.push_back("catB1");
	m.set("target", c.begin(), c.end());

	vector<string> r;
	m.get_categories("target", back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "catB", __FILE__, __LINE__);
	A(r.at(1) == "catB1", __FILE__, __LINE__);
}
END_TEST

namespace ml {
namespace map_impl {
namespace test {

TCase* create_add_tcase()
{
	TCase* tcase = tcase_create("add");
	tcase_add_test(tcase, should_add_categories);
	tcase_add_test(tcase, overwrite_categories);
	return tcase;
}

} // test
} // map_impl
} // ml
