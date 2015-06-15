#include <iterator>
#include <vector>
#include "string_utils.h"
#include <string_utils.h>

using namespace std;
using namespace su;

namespace {

class Split_string_fixture {
public:
	vector<string> results;
};

} // unnamed

START_TEST(should_return_an_item_without_any_sep)
{
	Split_string_fixture f;
	split_string("abc", "/", back_inserter(f.results));
	fail_unless(f.results.size() == 1, "count");
	fail_unless(f.results.at(0) == "abc", "content");
}
END_TEST

START_TEST(should_return_items_with_seps)
{
	Split_string_fixture f;
	split_string("ab/c/def/g", "/", back_inserter(f.results));
	fail_unless(f.results.size() == 4, "count");
	fail_unless(f.results.at(0) == "ab", "content 1");
	fail_unless(f.results.at(1) == "c", "content 2");
	fail_unless(f.results.at(2) == "def", "content 3");
	fail_unless(f.results.at(3) == "g", "content 4");
}
END_TEST

START_TEST(should_allow_multi_char_sep)
{
	Split_string_fixture f;
	split_string("ab#sep#c", "#sep#", back_inserter(f.results));
	fail_unless(f.results.size() == 2, "count");
	fail_unless(f.results.at(0) == "ab", "content 1");
	fail_unless(f.results.at(1) == "c", "content 2");
}
END_TEST

Suite* create_string_utils_test_suite()
{
	Suite* suite(suite_create("string_utils"));
	TCase* tcase(tcase_create("split_string"));
	tcase_add_test(tcase, should_return_an_item_without_any_sep);
	tcase_add_test(tcase, should_return_items_with_seps);
	tcase_add_test(tcase, should_allow_multi_char_sep);
	suite_add_tcase(suite, tcase);
	return suite;
}
