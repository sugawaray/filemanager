#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "core.h"
#include <map_impl.h>
#include <tests/fixture/db.h>
#include <tests/fixture/assert.h>

using namespace ml;
using namespace std;
using ::test::Db_fixture;

namespace {

template<class E, class T, class U>
inline void A(E e, T v1, U v2)
{
	::test::fixture::Assert(e, v1, v2);
}

} // unnamed

START_TEST(test_should_not_return_if_no_matches)
{
	Db_fixture f;
	vector<string> categories;
	categories.push_back("catA");
	vector<string> results;
	Map_impl map(f.get_dbfilepath());
	map.get(categories, back_inserter(results));
	fail_unless(results.size() == 0, "result count");
}
END_TEST

START_TEST(test_should_return_an_item_if_it_matches)
{
	Db_fixture f;
	vector<string> categories;
	categories.push_back("catA");
	Map_impl map1(f.get_dbfilepath());
	map1.set("target", categories.begin(), categories.end());
	Map_impl map2(f.get_dbfilepath());
	vector<string> results;
	map2.get(categories, back_inserter(results));
	fail_unless(results.size() == 1, "result count");
}
END_TEST

START_TEST(test_should_return_items_if_they_match)
{
	Db_fixture f;
	vector<string> categories;
	categories.push_back("catA");
	Map_impl map1(f.get_dbfilepath());
	map1.set("target1", categories.begin(), categories.end());
	map1.set("target2", categories.begin(), categories.end());

	Map_impl map2(f.get_dbfilepath());
	vector<string> results;
	map2.get(categories, back_inserter(results));
	fail_unless(results.size() == 2, "result count");
	sort(results.begin(), results.end());
	fail_unless(results.at(0) == "target1", "content1");
	fail_unless(results.at(1) == "target2", "content2");
}
END_TEST

START_TEST(test_should_return_an_item_if_it_matches_all_of_categories)
{
	Db_fixture f;
	vector<string> categories;
	categories.push_back("catA");
	Map_impl map1(f.get_dbfilepath());
	map1.set("target1", categories.begin(), categories.end());
	categories.push_back("catB");
	map1.set("target2", categories.begin(), categories.end());

	vector<string> results;
	Map_impl map2(f.get_dbfilepath());
	map2.get(categories, back_inserter(results));
	fail_unless(results.size() == 1, "count");
	fail_unless(results.at(0) == "target2", "result");
}
END_TEST

START_TEST(gets_categories_for_target)
{
	Db_fixture f;
	vector<string> c1;

	c1.push_back("catA");
	c1.push_back("catB");
	Map_impl m(f.get_dbfilepath());
	m.set("target1", c1.begin(), c1.end());

	vector<string> c2;
	c2.push_back("catC");
	c2.push_back("catD");
	m.set("target2", c2.begin(), c2.end());

	vector<string> r;
	m.get_categories("target1", back_inserter(r));
	A(r.size() == 2, __FILE__, __LINE__);
	sort(r.begin(), r.end());
	A(r.at(0) == "catA", __FILE__, __LINE__);
	A(r.at(1) == "catB", __FILE__, __LINE__);
}
END_TEST

namespace ml {
namespace test {
namespace map_impl {

TCase* create_tcase_for_core()
{
	TCase* tcase = tcase_create("core");
	tcase_add_test(tcase, test_should_not_return_if_no_matches);
	tcase_add_test(tcase, test_should_return_an_item_if_it_matches);
	tcase_add_test(tcase, test_should_return_items_if_they_match);
	tcase_add_test(tcase,
		test_should_return_an_item_if_it_matches_all_of_categories);
	tcase_add_test(tcase, gets_categories_for_target);
	return tcase;
}

} // map_impl
} // test
} // ml
