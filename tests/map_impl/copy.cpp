#include <iterator>
#include "copy.h"
#include "fixture/move_copy.h"

using std::back_inserter;
using std::function;
using std::string;
using ml::Map_impl;
using ml::map_impl::test::Move_copy_fixture;

namespace {

function<void (Map_impl&, const string&, const string&)>
get_delegate()
{
	using namespace std::placeholders;
	return std::bind(&Map_impl::copy, _1, _2, _3);
}

} // unnamed

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

START_TEST(should_not_remove_source)
{
	Move_copy_fixture f(get_delegate());
	f.test_source_removal(false);
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

TCase* create_tcase_for_copy()
{
	TCase* tcase(tcase_create("copy"));
	tcase_add_test(tcase,
		should_not_add_destination_without_entry_for_source);
	tcase_add_test(tcase,
		should_change_destination_without_entry_for_source);
	tcase_add_test(tcase, should_change_destination);
	tcase_add_test(tcase, should_not_remove_source);
	tcase_add_test(tcase, should_create_destination);
	return tcase;
}

} // map_impl
} // test
} // ml
