#include <iterator>
#include <string>
#include <vector>
#include <map_impl.h>
#include <receiver_impls.h>
#include <sql_value_spec.h>
#include <tests/fixture/db.h>
#include "remove_value_spec.h"

using test::Db_fixture;
using namespace ml;
using namespace std;

namespace {

class Fixture {
public:
	typedef back_insert_iterator<vector<string> > Inserter;
	typedef Receiver_by_inserter<Inserter> Receiver;
	Fixture()
		:	map(f.get_dbfilepath()) {
		categories.push_back("catA");
		categories.push_back("catB");
	}
	
	Map_impl& get_map() {
		return map;
	}

	const vector<string>&  get_categories() const {
		return categories;
	}

	Receiver get_receiver(vector<string>& buffer) const {
		auto inserter(back_inserter(buffer));
		return Receiver_fun(inserter);
	}
private:
	Db_fixture f;
	Map_impl map;
	vector<string> categories;
};

} // unnamed

START_TEST(should_not_remove_an_entry_if_it_does_not_match)
{
	Fixture f;
	f.get_map().set("dir1/file1", f.get_categories().begin(),
		f.get_categories().end());

	f.get_map().remove(Sql_middle_value_spec("file2"));

	vector<string> values;
	auto receiver(f.get_receiver(values));
	f.get_map().get_values(Sql_middle_value_spec(""), receiver);
	fail_unless(values.size() == 1, "count");
}
END_TEST

START_TEST(should_remove_an_entry_if_it_matches)
{
	Fixture f;
	f.get_map().set("dir1/file1", f.get_categories().begin(),
		f.get_categories().end());

	f.get_map().remove(Sql_middle_value_spec("file1"));

	vector<string> values;
	auto receiver(f.get_receiver(values));
	f.get_map().get_values(Sql_middle_value_spec(""), receiver);
	fail_unless(values.empty(), "should be empty");
}
END_TEST

namespace ml {
namespace test {
namespace map_impl {

TCase* create_tcase_for_remove_value_spec()
{
	TCase* tcase(tcase_create("remove_value_spec"));
	tcase_add_test(tcase, should_not_remove_an_entry_if_it_does_not_match);
	tcase_add_test(tcase, should_remove_an_entry_if_it_matches);
	return tcase;
}

} // map_impl
} // test
} // ml
