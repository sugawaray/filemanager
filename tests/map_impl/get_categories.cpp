#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "get_categories.h"
#include <map_impl.h>
#include <tests/fixture/db.h>

using namespace std;
using namespace ml;
using ::test::Db_fixture;

START_TEST(should_not_return_any_categories_when_categories_do_not_exist)
{
	Db_fixture f;
	Map_impl map_impl(f.get_dbfilepath());
	vector<string> categories;
	map_impl.get_categories(back_inserter(categories));

	fail_unless(categories.empty(), "should be empty");
}
END_TEST

START_TEST(should_return_categories_when_categories_exist)
{
	Db_fixture f;
	Map_impl map_impl(f.get_dbfilepath());
	vector<string> in_categories;
	in_categories.push_back("cat_a");
	in_categories.push_back("cat_b");
	map_impl.set("name1", in_categories.begin(), in_categories.end());
	vector<string> out_categories;
	map_impl.get_categories(back_inserter(out_categories));
	sort(out_categories.begin(), out_categories.end());

	fail_unless(out_categories.size() == 2, "count");
	fail_unless(out_categories.at(0) == "cat_a", "content 1");
	fail_unless(out_categories.at(1) == "cat_b", "content 2");
}
END_TEST

namespace ml {
namespace test {
namespace map_impl {

TCase* create_tcase_for_get_categories()
{
	TCase* tcase(tcase_create("get_categories"));
	tcase_add_test(tcase,
		should_not_return_any_categories_when_categories_do_not_exist);
	tcase_add_test(tcase,
		should_return_categories_when_categories_exist);
	return tcase;
}

} // map_impl
} // test
} // ml
