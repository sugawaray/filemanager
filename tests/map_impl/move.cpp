#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "fixture/move_copy.h"
#include "move.h"
#include <map_impl.h>
#include <tests/fixture/db.h>

using namespace ml;
using std::function;
using std::string;
using ::test::Db_fixture;
using map_impl::test::Move_copy_fixture;

namespace {

function<void (Map_impl&, const string&, const string&)>
get_delegate()
{
	using namespace std::placeholders;
	return std::bind(&Map_impl::move, _1, _2, _3);
}

} // unnamed

using namespace std;

START_TEST(should_not_add_destination_without_entry_for_source)
{
	Move_copy_fixture f(get_delegate());
	f.test_should_not_add_destination_without_entry();
}
END_TEST

START_TEST(should_change_destination_without_entry_for_source)
{
	Move_copy_fixture f(get_delegate());
	f.test_should_change_destination_without_entry();
}
END_TEST

START_TEST(should_change_destination)
{
	Move_copy_fixture f(get_delegate());
	f.test_should_change_destination();
}
END_TEST

START_TEST(should_remove_source)
{
	Move_copy_fixture f(get_delegate());
	f.test_source_removal(true);
}
END_TEST

START_TEST(should_create_destination)
{
	Move_copy_fixture f(get_delegate());
	f.test_should_create_destination();
}
END_TEST

namespace ml {
namespace test {
namespace map_impl {

TCase* create_tcase_for_move()
{
	TCase* tcase = tcase_create("move");
	tcase_add_test(tcase,
		should_not_add_destination_without_entry_for_source);
	tcase_add_test(tcase,
		should_change_destination_without_entry_for_source);
	tcase_add_test(tcase, should_change_destination);
	tcase_add_test(tcase, should_remove_source);
	tcase_add_test(tcase, should_create_destination);
	return tcase;
}

} // map_impl
} // test
} // ml
