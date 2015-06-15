#include <iterator>
#include <string>
#include <vector>
#include <map_impl.h>
#include "remove.h"
#include <tests/fixture/db.h>

using namespace ml;
using std::back_inserter;
using std::string;
using std::vector;
using ::test::Db_fixture;

START_TEST(should_be_ok_even_if_the_item_do_not_exist)
{
	Db_fixture f;
	Map_impl map_impl(f.get_dbfilepath());
	map_impl.remove("/dir1/dir2/file1");
}
END_TEST

START_TEST(should_remove_the_item)
{
	Db_fixture f;

	vector<string> categories;
	categories.push_back("cat1");
	categories.push_back("cat2");
	Map_impl map_impl(f.get_dbfilepath());
	map_impl.set("/dir1/dir2/file1", categories.begin(), categories.end());

	map_impl.remove("/dir1/dir2/file1");
	vector<string> results;
	map_impl.get(categories, back_inserter(results));
	fail_unless(results.empty(), "should be empty.");
}
END_TEST

namespace ml {
namespace test {
namespace map_impl {

TCase* create_tcase_for_remove()
{
	TCase* tcase(tcase_create("remove"));
	tcase_add_test(tcase, should_be_ok_even_if_the_item_do_not_exist);
	tcase_add_test(tcase, should_remove_the_item);
	return tcase;
}

} // map_impl
} // test
} // ml
