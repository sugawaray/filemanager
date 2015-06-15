#include <algorithm>
#include <iterator>
#include <vector>
#include <map_impl.h>
#include <receiver_impls.h>
#include <sql_value_spec.h>
#include <tests/fixture/db.h>
#include "get_values.h"

using test::Db_fixture;
using namespace ml;
using namespace std;

namespace {

class Fixture {
public:
	typedef back_insert_iterator<vector<string> > Inserter;

	Fixture() {
		categories1.push_back("catA");
		categories1.push_back("catB");
		categories2 = categories1;
		categories2.at(1) = "catC";
	}

	vector<string> values;

	string get_dbfilepath() {
		return f_db.get_dbfilepath();
	}

	Receiver_by_inserter<Inserter> get_receiver() {
		auto inserter = back_inserter(values);
		return Receiver_fun(inserter);
	}

	const vector<string>& get_categories_for_value1() const {
		return categories1;
	}

	const vector<string>& get_categories_for_value2() const {
		return categories2;
	}
private:
	Db_fixture f_db;
	vector<string> categories1;
	vector<string> categories2;
};

} // unnamed

START_TEST(should_not_return_any_values_when_empty)
{
	Fixture f;
	Map_impl map_impl(f.get_dbfilepath());

	auto receiver = f.get_receiver();
	map_impl.get_values(Sql_middle_value_spec("dir1/file1"), receiver);
	fail_unless(f.values.empty(), "should be empty.");
}
END_TEST

START_TEST(should_return_all_values)
{
	Fixture f;
	Map_impl map_impl(f.get_dbfilepath());
	map_impl.set("value1", f.get_categories_for_value1().begin(),
		f.get_categories_for_value1().end());
	map_impl.set("value2", f.get_categories_for_value2().begin(),
		f.get_categories_for_value2().end());

	auto receiver = f.get_receiver();
	map_impl.get_values(Sql_middle_value_spec("value"), receiver);
	fail_unless(f.values.size() == 2, "count");
	sort(f.values.begin(), f.values.end());
	fail_unless(f.values.at(0) == "value1", "value 1");
	fail_unless(f.values.at(1) == "value2", "value 2");
}
END_TEST

START_TEST(should_return_matching_values)
{
	Fixture f;
	Map_impl map_impl(f.get_dbfilepath());
	auto& categories_for_value1(f.get_categories_for_value1());
	auto& categories_for_value2(f.get_categories_for_value2());
	map_impl.set("value1", categories_for_value1.begin(),
		categories_for_value1.end());
	map_impl.set("value2", categories_for_value2.begin(),
		categories_for_value2.end());

	auto receiver(f.get_receiver());
	map_impl.get_values(Sql_middle_value_spec("value2"), receiver);
	fail_unless(f.values.size() == 1, "count");
	fail_unless(f.values.at(0) == "value2", "value");
}
END_TEST

namespace ml {
namespace test {
namespace map_impl {

TCase* create_tcase_for_get_values()
{
	TCase* tcase(tcase_create("get_values"));
	tcase_add_test(tcase, should_not_return_any_values_when_empty);
	tcase_add_test(tcase, should_return_all_values);
	tcase_add_test(tcase, should_return_matching_values);
	return tcase;
}

} // map_impl
} // test
} // ml
