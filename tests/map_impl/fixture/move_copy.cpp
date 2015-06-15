#include <algorithm>
#include <iterator>
#include <vector>
extern "C" {

#include <check.h>

} // extern "C"

#include "move_copy.h"

namespace ml {
namespace map_impl {
namespace test {

using std::back_inserter;
using std::find;
using std::function;
using std::string;
using std::vector;

Move_copy_fixture::Move_copy_fixture(Method_delegate delegate)
:	map(f_db.get_dbfilepath()),
	delegate(delegate)
{
}

void Move_copy_fixture::test_should_not_add_destination_without_entry()
{
	delegate(map, "source", "destination");
	map.get_categories(back_inserter(categories));
	fail_unless(categories.empty(), "should be empty");
}

void Move_copy_fixture::test_should_change_destination_without_entry()
{
	categories.push_back("catA");
	map.set("destination", categories.begin(), categories.end());
	map.set("destination", categories.begin(), categories.end());

	delegate(map, "source", "destination");

	categories.clear();
	map.get_categories(back_inserter(categories));
	fail_unless(categories.empty(), "should be empty");
}

void Move_copy_fixture::test_should_change_destination()
{
	categories.push_back("catA");
	categories.push_back("catC");
	map.set("source", categories.begin(), categories.end());

	categories.clear();
	categories.push_back("catB");
	map.set("destination", categories.begin(), categories.end());

	delegate(map, "source", "destination");

	categories.clear();
	categories.push_back("catA");
	categories.push_back("catC");
	vector<string> results;
	map.get(categories, back_inserter(results));
	fail_unless(find(results.begin(), results.end(), "destination")
		!= results.end(), "change destination category");
}

void Move_copy_fixture::test_source_removal(bool need_removal)
{
	categories.push_back("catA");
	map.set("source", categories.begin(), categories.end());

	categories.clear();
	categories.push_back("catB");
	map.set("destination", categories.begin(), categories.end());

	delegate(map, "source", "destination");

	categories.clear();
	categories.push_back("catA");
	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless((find(values.begin(), values.end(), "source")
		== values.end()) == need_removal, "should not be found.");
}

void Move_copy_fixture::test_should_create_destination()
{
	categories.push_back("catA");
	map.set("source", categories.begin(), categories.end());

	delegate(map, "source", "destination");

	vector<string> values;
	map.get(categories, back_inserter(values));
	fail_unless(find(values.begin(), values.end(), "destination")
		!= values.end(), "should be found.");
}

} // test
} // map_impl
} // ml

